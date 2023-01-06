// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_PieceInfoWidget.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "UserInterface/Match/Match_AbilityInfo.h"
#include "UserInterface/Match/Match_AbilityInfoPopup.h"
#include "UserInterface/Match/Match_ModifierList.h"

#include "Components/Overlay.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"

void UMatch_PieceInfoWidget::NativeConstruct()
{
    /* Bind the stats button to create modifier lists. */
    StatsButton->OnHovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnStatsHovered);
    StatsButton->OnUnhovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnStatsUnhovered);

    /* Bind the ability buttons to create an ability info widget pop-up. */
    ActiveAbilityBackgroundButton->OnHovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnActiveHovered);
    PassiveAbilityBackgroundButton->OnHovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnPassiveHovered);
    
    /* Bind the ability buttons to destroy the ability info widget pop-up. */
    ActiveAbilityBackgroundButton->OnUnhovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnActiveUnhovered);
    PassiveAbilityBackgroundButton->OnUnhovered.AddDynamic(this, &UMatch_PieceInfoWidget::OnPassiveUnhovered);

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

bool UMatch_PieceInfoWidget::UpdatePieceInfoWidget(AParentPiece* NewPiece, EAlignment Alignment)
{
    /* Tracks whether any displayed information has actually changed. */
    bool bInfoChanged = false;

    /* If the given piece is valid and the piece data table was found... */
    if (IsValid(NewPiece) && PieceDataTable)
    {
        /* Determine whether or not we can eventually enable buttons. We'll only consider enabling buttons on this
         * widget if it's this player's turn and the given piece is friendly. */
        bool bEnableButtons = false;
        const EPlayerStatus PlayerStatus = GetOwningPlayerState<AMatch_PlayerState>()->GetCurrentPlayerStatus();
        if ((PlayerStatus == E_SelectingPiece || PlayerStatus == E_SelectingAction || PlayerStatus == E_SelectingTarget_Move || PlayerStatus == E_SelectingTarget_ActiveAbility)
            && NewPiece->GetLocalAlignment() == E_Friendly)
                bEnableButtons = true;

        
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


            /* If there is no piece info widget currently displayed or the piece's stats have changed since its information was last opened, allow the opening animation to play. */
            if (DisplayedPiece == nullptr || NewPiece->GetCurrentStrength() != DisplayedPiece->GetCurrentStrength() || NewPiece->GetCurrentArmor() != DisplayedPiece->GetCurrentArmor())
                bInfoChanged = true;


            /* Update the piece's displayed current strength value. */
            DisplayedStrength->SetText(FText::FromString(FString::FromInt(NewPiece->GetCurrentStrength())));
            /* If this piece's current strength is the same as its base strength, highlight it in yellow if it has strength modifiers and white if it doesn't. */
            if (NewPiece->GetCurrentStrength() == PieceData->BaseStrength)
            {
                /* Check if the piece does have strength modifiers, but they actually just cancel out. */
                bool bModifiersCancel = false;
                for (FModifier const Modifier : NewPiece->GetTemporaryModifiers())
                {
                    if (Modifier.EffectedStat == Modifier.Strength)
                    {
                        bModifiersCancel = true;
                        break;
                    }
                }

                /* If the piece does have strength modifiers that cancel out, highlight the strength in yellow. */
                if (bModifiersCancel)
                {
                    DisplayedStrength->SetColorAndOpacity(FLinearColor(IneffectiveModifierColor));
                }
                /* If the piece doesn't have strength modifiers, don't highlight the text. */
                else
                {
                    DisplayedStrength->SetColorAndOpacity(FLinearColor(DefaultTextColor));
                }
            }
            /* If the piece's current strength is greater than its base strength, highlight it in green. */
            else if (NewPiece->GetCurrentStrength() > PieceData->BaseStrength)
            {
                DisplayedStrength->SetColorAndOpacity(FLinearColor(BuffColor));
            }
            /* If the piece's current strength is less than its base strength, highlight it in red. */
            else
            {
                DisplayedStrength->SetColorAndOpacity(FLinearColor(DebuffColor));
            }


            /* Update the piece's displayed current armor value. */
            DisplayedArmor->SetText(FText::FromString(FString::FromInt(NewPiece->GetCurrentArmor())));
            /* If this piece's current armor is the same as its base armor, highlight it in yellow if it has armor modifiers and white if it doesn't. */
            if (NewPiece->GetCurrentArmor() == PieceData->BaseArmor)
            {
                /* Check if the piece does have armor modifiers, but they actually just cancel out. */
                bool bModifiersCancel = false;
                for (FModifier const Modifier : NewPiece->GetTemporaryModifiers())
                {
                    if (Modifier.EffectedStat == Modifier.Armor)
                    {
                        bModifiersCancel = true;
                        break;
                    }
                }

                /* If the piece does have armor modifiers that cancel out, highlight the armor in yellow. */
                if (bModifiersCancel)
                {
                    DisplayedArmor->SetColorAndOpacity(FLinearColor(IneffectiveModifierColor));
                }
                /* If the piece doesn't have armor modifiers, don't highlight the text. */
                else
                {
                    DisplayedArmor->SetColorAndOpacity(FLinearColor(DefaultTextColor));
                }
            }
            /* If the piece's current armor is greater than its base armor, highlight it in green. */
            else if (NewPiece->GetCurrentArmor() > PieceData->BaseArmor)
            {
                DisplayedArmor->SetColorAndOpacity(FLinearColor(BuffColor));
            }
            /* If the piece's current armor is less than its base armor, highlight it in red. */
            else
            {
                DisplayedArmor->SetColorAndOpacity(FLinearColor(DebuffColor));
            }


            /* Reveal and enable the move button if buttons are enabled and a move action can be used. */
            if (bEnableButtons)
            {
                MoveButton->SetVisibility(ESlateVisibility::Visible);
                MoveButton->SetIsEnabled(!GetOwningPlayerState<AMatch_PlayerState>()->GetMoveActionUsed());
            }
            /* Hide the move button if buttons are not enabled. */
            else
            {
                MoveButton->SetVisibility(ESlateVisibility::Collapsed);
            }


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
                ActiveAbilityBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

                /* Reveal and enable the ability button if buttons are enabled and an ability action can be used. */
                if (bEnableButtons)
                {
                    ActiveButton->SetVisibility(ESlateVisibility::Visible);
                    ActiveButton->SetIsEnabled(!GetOwningPlayerState<AMatch_PlayerState>()->GetAbilityActionUsed());
                }
                /* Hide the ability button if buttons are not enabled. */
                else
                {
                    ActiveButton->SetVisibility(ESlateVisibility::Collapsed);
                }

                /* Reveal and update the usage bars if this piece's active ability has limited uses. */
                if (PieceData->ActiveUses > 0)
                {
                    EmptyBars(true, true, PieceData->ActiveUses - NewPiece->GetActiveUses());
                }
                /* Hide the usage bars if this piece's active ability has unlimited uses. */
                else
                {
                    /* Hide the usage bars. */
                    EmptyBars(false, true, 0);
                }

                /* Update the cooldown timer if the active ability's cooldown is active. */
                if (NewPiece->GetActiveCD() > 0)
                {
                    /* Don't display a cooldown counter if the piece doesn't have any more ability uses left. */
                    if ((PieceData->ActiveUses > 0 && NewPiece->GetActiveUses() > 0) || PieceData->ActiveUses == 0)
                    {
                        /* Update and display the cooldown counter. */
                        DisplayedActiveCD->SetText(FText::FromString(FString::FromInt(NewPiece->GetActiveCD())));
                        DisplayedActiveCD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                    }
                }
                /* Hide the cooldown counter if the active ability is not on cooldown.    */
                else
                {
                    DisplayedActiveCD->SetVisibility(ESlateVisibility::Hidden);
                }

                /* If this piece's active ability is not on cooldown, and either has limited uses and has uses left or has unlimited uses, then it can be used. */
                if (NewPiece->GetActiveCD() == 0 && ((PieceData->ActiveUses > 0 && NewPiece->GetActiveUses() > 0) || PieceData->ActiveUses == 0))
                {
                    /* Hide the cooldown indicator (that doubles as the "no uses left" indicator). */
                    ActiveCooldownIndicator->SetVisibility(ESlateVisibility::Hidden);
                    // /* Enable the "use active ability" button. */
                    // ActiveButton->SetIsEnabled(true);
                }
                /* If this ability can't be used. */
                else
                {
                    /* Reveal the active cooldown indicator, greying out the ability icon. */
                    ActiveCooldownIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                    /* Disable the "use active ability" button. */
                    ActiveButton->SetIsEnabled(false);
                }
            }
            /* If this piece doesn't have an active ability... */
            else
            {
                /* Hide all of the active ability widgets. */
                ActiveAbilityBox->SetVisibility(ESlateVisibility::Collapsed);
                /* Hide the active ability button. */
                ActiveButton->SetVisibility(ESlateVisibility::Collapsed);
            }


            /* If this piece has an passive ability... */
            if (PieceData && PieceData->PassiveName != "")
            {
                /* Reveal all of the passive ability widgets. */
                PassiveAbilityBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

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
                EmptyBars(true, false, (PieceData->PassiveUses) - (NewPiece->GetPassiveUses()));
            }
            /* If this piece doesn't have a passive ability... */
            else
            {
                /* Hide all of the passive ability widgets. */
                PassiveAbilityBox->SetVisibility(ESlateVisibility::Collapsed);
            }


            /* Disable both buttons if this player is currently in a sequence that prevents them from taking actions. */
            if (GetOwningPlayerState<AMatch_PlayerState>()->GetIsInSequence())
            {
                MoveButton->SetIsEnabled(false);
                ActiveButton->SetIsEnabled(false);
            }
        }

        /* If the selected piece is friendly... */
        if (Alignment == E_Friendly)
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


        /* Set the displayed piece pointer to the passed piece for use in other functions. */
        DisplayedPiece = NewPiece;
    }

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

void UMatch_PieceInfoWidget::RefreshWidget()
{
    /* Refresh the information in this widget. */
    UpdatePieceInfoWidget(DisplayedPiece, DisplayedPiece->GetLocalAlignment());
}

bool UMatch_PieceInfoWidget::AreStatsHovered() const
{
    return StatsButton->IsHovered();
}

void UMatch_PieceInfoWidget::OnStatsHovered()
{
    /* Get the currently displayed piece's active modifiers and alignment. */
    const TArray<FModifier> ActiveModifiers = DisplayedPiece->GetTemporaryModifiers();
    const EAlignment PieceAlignment = DisplayedPiece->GetLocalAlignment();

    /* If this piece has any currently active modifiers. */
    if (ActiveModifiers.Num() > 0 && ModifierListClass)
    {
        /* Create and populate a list of modifier widgets. */
        ModifierList = Cast<UMatch_ModifierList>(CreateWidget<UUserWidget>(GetWorld(), ModifierListClass, FName("Modifier List Widget")));
        ModifierList->PopulateModifierList(this, ActiveModifiers, PieceAlignment == E_Friendly);

        /* Attach the list to the stats button. */
        ModifierListWrapper->AddChild(ModifierList);

        /* Check if this piece's strength is affected by a modifier. If not, then this piece's modifiers are only
         * affecting its armor. */
        bool bStrengthModified = false;
        for (const FModifier Mod : ActiveModifiers)
        {
            if (Mod.StrengthChange != 0)
            {
                bStrengthModified = true;
                break;
            }
        }

        /* If this piece's armor is the only modified stat, move it halfway down the stats box to position it above the
         * armor stat. Otherwise, leave it at the top, by the strength stat. */
        const float InitialOffset = bStrengthModified ? 0.0f : StatsButtonOverlay->GetDesiredSize().Y / 2.0f;
        
        /* Offset the widget based on the modified stat and whether or not the displayed piece is friendly. */
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ModifierList->ModifierListOverlay->Slot);
        CanvasSlot->SetPosition(FVector2D(PieceAlignment == E_Friendly ? StatsButtonOverlay->GetDesiredSize().X - 20.0f : 20.0f, -StatsButtonOverlay->GetDesiredSize().Y + InitialOffset + 20.0f));

        /* Align the pop-up to the left if it's for a friendly piece. Align it to the right if it's for an enemy piece. */
        CanvasSlot->SetAlignment(PieceAlignment == E_Friendly ? FVector2D(0.0f, 1.0f) : FVector2D(1.0f, 1.0f));
    }
}

void UMatch_PieceInfoWidget::OnStatsUnhovered()
{
    /* Destroy the list of current stat modifiers. */
    BP_OnStatsUnhovered();
}

void UMatch_PieceInfoWidget::OnActiveHovered()
{
    /* If the piece data has been found for this piece and the ability info class is set... */
    if (PieceData && DisplayedPiece->ActiveAbilityInfoWidget)
    {
        /* Store whether the displayed piece is friendly or not so we don't have to keep checking. */
        const EAlignment PieceAlignment = DisplayedPiece->GetLocalAlignment();

        /* Create a new ability info pop-up widget and update its information. */
        AbilityInfoPopup = Cast<UMatch_AbilityInfoPopup>(CreateWidget<UUserWidget>(GetWorld(), DisplayedPiece->ActiveAbilityInfoWidget, FName("Ability Info Pop-Up")));
        AbilityInfoPopup->SetUpWidget(DisplayedPiece, true, PieceAlignment == E_Friendly);
        /* Attach the widget to the active ability icon. */
        ActiveAbilityPopupWrapper->AddChild(AbilityInfoPopup);

        /* Offset the widget based on the size of the ability icon depending on whether or not the displayed piece is friendly. */
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(AbilityInfoPopup->AbilityInfoOverlay->Slot);
        CanvasSlot->SetPosition(FVector2D(PieceAlignment == E_Friendly ? ActiveAbilityBackgroundButton->GetDesiredSize().X - 20.0f : 20.0f, -ActiveAbilityBackgroundButton->GetDesiredSize().Y + 20.0f));
        /* Align the pop-up to the left if it's for a friendly piece. Align it to the right if it's for an enemy piece. */
        CanvasSlot->SetAlignment(PieceAlignment == E_Friendly ? FVector2D(0.0f, 1.0f) : FVector2D(1.0f, 1.0f));
    }

    /* Highlight all valid active ability targets. */
    for (AActor* Target : DisplayedPiece->GetValidActiveAbilityTargets())
    {
        if (Target->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
        {
            Cast<ITargetInterface>(Target)->HighlightTarget();
        }
    }
}

void UMatch_PieceInfoWidget::OnPassiveHovered()
{
    /* If the piece data has been found for this piece and the ability info class is set... */
    if (PieceData && DisplayedPiece->PassiveAbilityInfoWidget)
    {
        /* Store whether the displayed piece is friendly or not so we don't have to keep checking. */
        const EAlignment PieceAlignment = DisplayedPiece->GetLocalAlignment();

        /* Create a new ability info pop-up widget and update its information. */
        AbilityInfoPopup = Cast<UMatch_AbilityInfoPopup>(CreateWidget<UUserWidget>(GetWorld(), DisplayedPiece->PassiveAbilityInfoWidget, FName("Ability Info Pop-Up")));
        AbilityInfoPopup->SetUpWidget(DisplayedPiece, false, PieceAlignment == E_Friendly);
        /* Attach the widget to the passive ability icon. */
        PassiveAbilityPopupWrapper->AddChild(AbilityInfoPopup);

        /* Offset the widget based on the size of the ability icon. */
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(AbilityInfoPopup->AbilityInfoOverlay->Slot);
        CanvasSlot->SetPosition(FVector2D(PieceAlignment == E_Friendly ? PassiveAbilityBackgroundButton->GetDesiredSize().X - 20.0f : 20.0f, -PassiveAbilityBackgroundButton->GetDesiredSize().Y + 20.0f));
        /* Align the pop-up to the left if it's for a friendly piece. Align it to the right if it's for an enemy piece. */
        CanvasSlot->SetAlignment(PieceAlignment == E_Friendly ? FVector2D(0.0f, 1.0f) : FVector2D(1.0f, 1.0f));
    }

    /* Highlight all valid passive ability targets. */
    for (AActor* Target : DisplayedPiece->GetValidPassiveAbilityTargets())
    {
        if (Target->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
        {
            Cast<ITargetInterface>(Target)->HighlightTarget();
        }
    }
}

void UMatch_PieceInfoWidget::OnActiveUnhovered()
{
    /* Destroy the ability info widget if it's currently displayed. */
    if (IsValid(AbilityInfoPopup))
    {
        AbilityInfoPopup->RemoveFromParent();
        AbilityInfoPopup = nullptr;
    }

    /* Remove the highlight from every valid active ability target. */
    for (AActor* Target : DisplayedPiece->GetValidActiveAbilityTargets())
    {
        if (Target->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
        {
            Cast<ITargetInterface>(Target)->RemoveTargetHighlight();
        }
    }
}

void UMatch_PieceInfoWidget::OnPassiveUnhovered()
{
    /* Destroy the ability info widget if it's currently displayed. */
    if (IsValid(AbilityInfoPopup))
    {
        AbilityInfoPopup->RemoveFromParent();
        AbilityInfoPopup = nullptr;
    }

    /* Remove the highlight from every valid passive ability target. */
    for (AActor* Target : DisplayedPiece->GetValidPassiveAbilityTargets())
    {
        if (Target->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
        {
            Cast<ITargetInterface>(Target)->RemoveTargetHighlight();
        }
    }
}

void UMatch_PieceInfoWidget::OnMoveClicked()
{
    /* Safety check. Ensure that the player is in the correct state to be taking actions. */
    const AMatch_PlayerState* PlayerStatePtr = GetOwningPlayerState<AMatch_PlayerState>();
    if (IsValid(PlayerStatePtr) && PlayerStatePtr->GetCurrentPlayerStatus() == E_SelectingAction)
    {
        /* Clear the piece info widgets but keep the currently selected piece. */
        GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(false, false, false, false, true);
        
        /* Set the player's state to be selecting a place to move. */
        GetOwningPlayerState<AMatch_PlayerState>(false)->SetPlayerStatus(E_SelectingTarget_Move);

        /* Create a new move confirmation widget. */
        GetOwningPlayer<AMatch_PlayerController>()->CreateMoveConfirmationWidget(false, DisplayedPiece);
        
        /* Make sure that there's a valid displayed piece. */
        if (IsValid(DisplayedPiece))
        {
            /* Set a new highlight for every tile that this piece can move to to show the player their options. */
            for (ABoardTile* Tile : DisplayedPiece->GetValidMoveTiles())
            {
                /* Highlight the tile depending on its alignment and occupancy. */
                Tile->HighlightTarget();

                // /* If this tile is occupied by an enemy piece, highlight it as a valid attack target. */
                // if (IsValid(Tile->GetOccupyingPiece()) && Tile->GetOccupyingPiece()->GetLocalAlignment() == E_Hostile)
                // {
                //     Tile->UpdateEmissiveHighlight(true, Tile->DefaultHighlightPlayRate, Tile->Highlight_Enemy);
                // }
                // /* If the tile is empty, highlight it as a valid target destination. */
                // else if (!IsValid(Tile->GetOccupyingPiece()))
                // {
                //     Tile->UpdateEmissiveHighlight(true, Tile->DefaultHighlightPlayRate, Tile->Highlight_ValidUnoccupiedTile);
                // }
            }
        }
    }
    /* If the player is somehow not in the correct state, refresh this widget to properly reflect their actual state. */
    else
    {
        RefreshWidget();
    }
}

void UMatch_PieceInfoWidget::OnUseActiveClicked()
{
    /* Safety check. Ensure that the player is in the correct state to be taking actions. */
    const AMatch_PlayerState* PlayerStatePtr = GetOwningPlayerState<AMatch_PlayerState>();
    if (IsValid(PlayerStatePtr) && PlayerStatePtr->GetCurrentPlayerStatus() == E_SelectingAction)
    {
        /* Clear the piece info widgets but keep the currently selected piece. */
        GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(false, false, false, false, true);

        /* Set the player's state to be selecting targets for an active ability. */
        GetOwningPlayerState<AMatch_PlayerState>(false)->SetPlayerStatus(E_SelectingTarget_ActiveAbility);

        /* Make sure that there's a valid displayed piece. */
        if (IsValid(DisplayedPiece))
        {
            /* Call the ability-specific code used when the active ability is selected. By default, this just highlights
             * all of the valid targets. */
            DisplayedPiece->OnActiveClicked();
        }
    }
    /* If the player is somehow not in the correct state, refresh this widget to properly reflect their actual state. */
    else
    {
        RefreshWidget();
    }
}