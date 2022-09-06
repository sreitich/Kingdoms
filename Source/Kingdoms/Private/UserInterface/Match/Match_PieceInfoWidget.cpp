// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_PieceInfoWidget.h"

#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"
#include "UserInterface/Match/Match_AbilityInfo.h"
#include "Board/BoardTile.h"
#include "Pieces/Mages/Pyromancer.h"
#include "Pieces/Soldiers/Knight.h"

#include "Runtime/UMG/Public/UMG.h"

void UMatch_PieceInfoWidget::NativeConstruct()
{
    /* Bind the ability buttons to create an ability info widget pop-up. */
    ActiveAbilityBackgroundButton->OnHovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnActiveHovered);
    PassiveAbilityBackgroundButton->OnHovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnPassiveHovered);
    
    /* Bind the ability buttons to destroy the ability info widget pop-up. */
    ActiveAbilityBackgroundButton->OnUnhovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnAbilityUnhovered);
    PassiveAbilityBackgroundButton->OnUnhovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnAbilityUnhovered);

    /* Bind the "move" and "use active" buttons to activate their effects. */
    MoveButton->OnClicked.AddDynamic(this, &UMatch_PieceInfoWidget::OnMoveClicked);
    ActiveButton->OnClicked.AddDynamic(this, &UMatch_PieceInfoWidget::OnUseActiveClicked);

    /* Bind the OpeningAnimFinished and ClosingAnimFinished functions to their respective delegates. */
    OpenFinishedDelegate.BindDynamic(this, &UMatch_PieceInfoWidget::OpeningAnimFinished);
    CloseFinishedDelegate.BindDynamic(this, &UMatch_PieceInfoWidget::ClosingAnimFinished);
	
    /* Bind functions to enable or hide this widget to when the opening or closing animations finish playing. */
    BindToAnimationFinished(PieceInfoOpenAnim, OpenFinishedDelegate);
    BindToAnimationFinished(PieceInfoCloseAnim, CloseFinishedDelegate);
}

void UMatch_PieceInfoWidget::OpeningAnimFinished()
{
    /* Enable this widget. */
    SetVisibility(ESlateVisibility::Visible);
}

void UMatch_PieceInfoWidget::ClosingAnimFinished()
{
    /* Hide this widget. */
    SetVisibility(ESlateVisibility::Hidden);
}

bool UMatch_PieceInfoWidget::UpdatePieceInfoWidget(AParentPiece* NewPiece, bool bIsFriendly, bool bEnableButtons)
{
    /* Tracks whether any displayed information has actually changed. */
    bool bInfoChanged = false;

    /* If the piece data table was found... */
    if (PieceDataTable)
    {
        /* Get this piece's row from the piece data. */
        static const FString ContextString(TEXT("Piece Data Struct"));
        PieceData = PieceDataTable->FindRow<FPieceDataStruct>(NewPiece->PieceID, ContextString, true);

        /* If the data table row was found... */
        if (PieceData)
        {
            /* Display constant values retrieved from the piece data table. */
            DisplayedPhoto->SetBrushFromTexture(PieceData->PieceInfoPhoto, true);

            /* If there is no piece info widget displayed or if the new piece is not the same as the old one, 
            allow the opening animation to play to indicate that it's a different piece. */
            if (DisplayedPiece == nullptr || !NewPiece->GetFullName().Equals(DisplayedPiece->GetFullName()))
                bInfoChanged = true;
            
            DisplayedPieceName->SetText(FText::FromString(PieceData->PieceName.ToUpper()));

            DisplayedPieceRarity->SetText(StaticEnum<EPieceRarity>()->GetDisplayNameTextByValue(PieceData->Rarity).ToUpper());

            DisplayedPieceClass->SetText(StaticEnum<EPieceClass>()->GetDisplayNameTextByValue(PieceData->ClassType).ToUpper());


            /* If this piece has an active ability... */
            if (PieceData->ActiveName != "")
            {
                /* Update the active ability icon. */
                ActiveAbilityIcon->SetBrushFromTexture(PieceData->ActiveIcon);

                /* Set the correct number of total uses. */
                ResetBars(true, PieceData->ActiveUses);
            }

            /* If this piece has a passive ability... */
            if (PieceData->PassiveName != "")
            {
                /* Update the passive ability icon. */
                PassiveAbilityIcon->SetBrushFromTexture(PieceData->PassiveIcon);

                /* Set the correct number of total uses. */
                ResetBars(false, PieceData->PassiveUses);
            }

            /* Set the piece's photo's background to match the corresponding rarity. */
            switch (PieceData->Rarity)
            {
                case E_Superior:
                    DisplayedPhotoBackground->SetBrushFromTexture(Background_Superior, true);
                    break;
                case E_Heroic:
                    DisplayedPhotoBackground->SetBrushFromTexture(Background_Heroic, true);
                    break;
                case E_Champion:
                    DisplayedPhotoBackground->SetBrushFromTexture(Background_Champion, true);
                    break;
                /* If the piece's rarity is "basic" or isn't valid. */
                default:
                    DisplayedPhotoBackground->SetBrushFromTexture(Background_Basic, true);
            }
        }

        /* If the passed piece is valid... */
        if (IsValid(NewPiece))
        {
            /* If there is no piece info widget currently displayed or the piece's stats have changed since its information was last opened, allow the opening animation to play. */
            if (DisplayedPiece == nullptr || NewPiece->GetCurrentStrength() != DisplayedPiece->GetCurrentStrength() || NewPiece->GetCurrentArmor() != DisplayedPiece->GetCurrentArmor())
                    bInfoChanged = true;
            
            /* Update stats with the current values (so that modifiers are counted). */
            DisplayedStrength->SetText(FText::FromString(FString::FromInt(NewPiece->GetCurrentStrength())));

            DisplayedArmor->SetText(FText::FromString(FString::FromInt(NewPiece->GetCurrentArmor())));


            /* If this piece has both a passive and active ability... */
            if (PieceData && PieceData->PassiveName != "" && PieceData->ActiveName != "")
            {
                /* Align both ability boxes together. */
                Cast<UHorizontalBoxSlot>(PassiveAbilityBox->Slot)->SetPadding(FMargin(0.0f, 15.0f, 15.0f, 0.0f));
                Cast<UHorizontalBoxSlot>(ActiveAbilityBox->Slot)->SetPadding(FMargin(0.0f, 15.0f, 15.0f, 0.0f));
            }
            /* If this piece only has a passive ability... */
            else if (PieceData && PieceData->PassiveName != "")
            {
                /* Center the passive ability. */
                Cast<UHorizontalBoxSlot>(PassiveAbilityBox->Slot)->SetPadding(FMargin(0.0f, 15.0f, 57.0f, 0.0f));
            }
            /* If this piece only has an active ability... */
            else if (PieceData && PieceData->ActiveName != "")
            {
                /* Center the active ability. */
                Cast<UHorizontalBoxSlot>(ActiveAbilityBox->Slot)->SetPadding(FMargin(0.0f, 15.0f, 57.0f, 0.0f));
            }


            /* If this piece has an active ability... */
            if (PieceData && PieceData->ActiveName != "")
            {
                /* Reveal all of the active ability widgets. */
                ActiveAbilityBox->SetVisibility(ESlateVisibility::Visible);

                /* If the active ability's cooldown is active... */
                if (NewPiece->GetActiveCD() > 0)
                {
                    /* Update and display the current cooldown. */
                    DisplayedActiveCD->SetText(FText::FromString(FString::FromInt(NewPiece->GetActiveCD())));
                    DisplayedActiveCD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

                    /* Reveal the active cooldown indicator, greying out the active ability icon. Disable test visibility so that it doesn't block the button. */
                    ActiveCooldownIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                }
                /* If the active ability can be used... */
                else
                {
                    /* Hide the active cooldown indicator and the cooldown turn counter. */
                    ActiveCooldownIndicator->SetVisibility(ESlateVisibility::Hidden);
                    DisplayedActiveCD->SetVisibility(ESlateVisibility::Hidden);
                }

                /* Update the number of uses remaining. */
                EmptyBars(true, (PieceData->ActiveUses) - (NewPiece->GetActiveUses()));
            }
            /* If this piece doesn't have an active ability... */
            else
            {
                /* Hide all of the active ability widgets. */
                ActiveAbilityBox->SetVisibility(ESlateVisibility::Collapsed);
            }


            /* If this piece has an passive ability... */
            if (PieceData && PieceData->PassiveName != "")
            {
                /* Reveal all of the passive ability widgets. */
                PassiveAbilityBox->SetVisibility(ESlateVisibility::Visible);

                /* If the passive ability's cooldown is active... */
                if (NewPiece->GetPassiveCD() > 0)
                {
                    /* Update and display the current cooldown. */
                    DisplayedPassiveCD->SetText(FText::FromString(FString::FromInt(NewPiece->GetPassiveCD())));
                    DisplayedPassiveCD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

                    /* Reveal the passive cooldown indicator, greying out the passive ability icon. Disable test visibility so that it doesn't block the button. */
                    PassiveCooldownIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                }
                /* If the passive ability can be used... */
                else
                {
                    /* Hide the passive cooldown indicator and the cooldown turn counter. */
                    PassiveCooldownIndicator->SetVisibility(ESlateVisibility::Hidden);
                    DisplayedPassiveCD->SetVisibility(ESlateVisibility::Hidden);
                }

                /* Update the number of uses remaining. */
                EmptyBars(false, (PieceData->PassiveUses) - (NewPiece->GetPassiveUses()));
            }
            /* If this piece doesn't have a passive ability... */
            else
            {
                /* Hide all of the passive ability widgets. */
                PassiveAbilityBox->SetVisibility(ESlateVisibility::Collapsed);
            }
        }

        /* If buttons need to be enabled for the user... */
        if (bEnableButtons)
        {
            /* Enable and reveal the buttons. */
            MoveButton->SetVisibility(ESlateVisibility::Visible);
            ActiveButton->SetVisibility(ESlateVisibility::Visible);
        }
        /* If buttons need to be disabled for the user... */
        else
        {
            /* Disable and hide the buttons. */
            MoveButton->SetVisibility(ESlateVisibility::Hidden);
            ActiveButton->SetVisibility(ESlateVisibility::Hidden);
        }

        /* If the selected piece is friendly... */
        if (bIsFriendly)
        {
            /* Change the background textures to be friendly. */
            ActiveAbilityBackgroundButton->WidgetStyle.Normal.SetResourceObject(ActiveBackground_Friendly);
            ActiveAbilityBackgroundButton->WidgetStyle.Hovered.SetResourceObject(ActiveBackground_Friendly);
            ActiveAbilityBackgroundButton->WidgetStyle.Pressed.SetResourceObject(ActiveBackground_Friendly);
            ActiveAbilityBackgroundButton->WidgetStyle.Disabled.SetResourceObject(ActiveBackground_Friendly);

            PassiveAbilityBackgroundButton->WidgetStyle.Normal.SetResourceObject(PassiveBackground_Friendly);
            PassiveAbilityBackgroundButton->WidgetStyle.Hovered.SetResourceObject(PassiveBackground_Friendly);
            PassiveAbilityBackgroundButton->WidgetStyle.Pressed.SetResourceObject(PassiveBackground_Friendly);
            PassiveAbilityBackgroundButton->WidgetStyle.Disabled.SetResourceObject(PassiveBackground_Friendly);
        }
        /* If the selected piece is hostile... */
        else
        {
            /* Change the background textures to be hostile. */
            ActiveAbilityBackgroundButton->WidgetStyle.Normal.SetResourceObject(ActiveBackground_Enemy);
            ActiveAbilityBackgroundButton->WidgetStyle.Hovered.SetResourceObject(ActiveBackground_Enemy);
            ActiveAbilityBackgroundButton->WidgetStyle.Pressed.SetResourceObject(ActiveBackground_Enemy);
            ActiveAbilityBackgroundButton->WidgetStyle.Disabled.SetResourceObject(ActiveBackground_Enemy);

            PassiveAbilityBackgroundButton->WidgetStyle.Normal.SetResourceObject(PassiveBackground_Enemy);
            PassiveAbilityBackgroundButton->WidgetStyle.Hovered.SetResourceObject(PassiveBackground_Enemy);
            PassiveAbilityBackgroundButton->WidgetStyle.Pressed.SetResourceObject(PassiveBackground_Enemy);
            PassiveAbilityBackgroundButton->WidgetStyle.Disabled.SetResourceObject(PassiveBackground_Enemy);
        }
    }

    /* Set the displayed piece pointer to the passed piece for use in other functions. */
    DisplayedPiece = NewPiece;

    /* Return whether any displayed information has changed, if this info widget was already open. */
    return bInfoChanged;
}

void UMatch_PieceInfoWidget::PlayOpenCloseAnim(bool bOpen, float StartTime, int32 NumLoops, EUMGSequencePlayMode::Type PlayMode, float Speed, bool bRestoreState)
{
    /* Play the opening or closing animation with the given parameters. */
    PlayAnimation
    (
        bOpen ? PieceInfoOpenAnim : PieceInfoCloseAnim,
        StartTime,
        NumLoops,
        PlayMode,
        Speed,
        bRestoreState
    );
}

void UMatch_PieceInfoWidget::OnActiveHovered()
{
    /* If the piece data has been found for this piece and the ability info class is set... */
    if (PieceData && AbilityInfoClass)
    {
        /* Create a new ability info pop-up widget and update its name and description. */
        AbilityInfoWidget = Cast<UMatch_AbilityInfo>(CreateWidget<UUserWidget>(GetWorld(), AbilityInfoClass, FName("Ability Info")));
        AbilityInfoWidget->AddToViewport(0);
        AbilityInfoWidget->UpdateAbilityInfo(PieceData->ActiveName, PieceData->ActiveDes);
    }
}

void UMatch_PieceInfoWidget::OnPassiveHovered()
{
    /* If the piece data has been found for this piece and the ability info class is set... */
    if (PieceData && AbilityInfoClass)
    {
        /* Create a new ability info pop-up widget and update its name and description. */
        AbilityInfoWidget = Cast<UMatch_AbilityInfo>(CreateWidget<UUserWidget>(GetWorld(), AbilityInfoClass, FName("Ability Info")));
        AbilityInfoWidget->AddToViewport(0);
        AbilityInfoWidget->UpdateAbilityInfo(PieceData->PassiveName, PieceData->PassiveDes);
    }
}

void UMatch_PieceInfoWidget::OnAbilityUnhovered()
{
    /* If an ability info widget is currently displayed... */
    if (IsValid(AbilityInfoWidget))
    {
        /* Destroy the widget. */
        AbilityInfoWidget->RemoveFromParent();
    }
}

void UMatch_PieceInfoWidget::OnMoveClicked()
{
    /* CLear the piece info widgets but keep the currently selected piece. */
    GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(false);

    /* Instantly change the player status for the local client so that they don't have to wait for the server to update
     * their status before choosing a target. */
    GetOwningPlayerState<AMatch_PlayerState>(false)->SetLocalPlayerStatus(E_SelectingTarget_Move);
    
    /* Set the player's state to be selecting a place to move. */
    GetOwningPlayerState<AMatch_PlayerState>(false)->Server_SetPlayerStatus(E_SelectingTarget_Move);

    /* Make sure that there's a valid displayed piece. */
    if (IsValid(DisplayedPiece))
    {
        /* Set a new highlight for every tile that this piece can move to to show the player their options for
         * actions. */
        for (ABoardTile* Tile : DisplayedPiece->GetValidTiles())
        {
            /* If this tile is occupied by a friendly piece... */
            if (IsValid(Tile->GetOccupyingPiece()) && Tile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled())
            {
                Tile->Highlight->SetMaterial(0, Tile->Highlight_ValidFriendly);
            }
            /* If this tile is occupied by an enemy piece... */
            else if (IsValid(Tile->GetOccupyingPiece()))
            {
                Tile->Highlight->SetMaterial(0, Tile->Highlight_ValidEnemy);
            }
            /* If the tile is empty... */
            else
            {
                Tile->Highlight->SetMaterial(0, Tile->Highlight_ValidMove);
            }
        }
    }
}

void UMatch_PieceInfoWidget::OnUseActiveClicked()
{

}