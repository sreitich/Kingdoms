// Copyright Samuel Reitich 2022.

#include "Framework/Match/Match_PlayerController.h"

#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"
#include "UserInterface/Match/Match_AttackConfirmation.h"
#include "UserInterface/Match/Match_AttackGraphic.h"
#include "UserInterface/Match/Match_BaseWidget.h"
#include "UserInterface/Match/Match_MoveConfirmation.h"
#include "UserInterface/Match/Match_PieceInfoWidget.h"
#include "UserInterface/MatchSetup/MatchSetup_PlacePieces.h"
#include "UserInterface/MatchSetup/MatchSetup_SelectArmy.h"

#include "Board/BoardTile.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Pieces/ParentPiece.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

AMatch_PlayerController::AMatch_PlayerController()
{
    /* Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it. */
    PrimaryActorTick.bCanEverTick = true;

    /* Enable mouse controls. */
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    /* Set up this actor's components. */
    ServerCommunicationComponent = CreateDefaultSubobject<UServerCommunicationComponent>(TEXT("Server Communication Component"));
}

void AMatch_PlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateTimer();
}

void AMatch_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    /* If this controller belongs to this machine... */
    if (IsLocalPlayerController())
    {
        /* If the army selection widget class is set... */
        if (IsValid(MatchSetup_SelectArmyClass))
        {
            /* Enable input with both the world and the user interface, and prevent the viewport from ever capturing or locking the mouse. */
            UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, nullptr, EMouseLockMode::DoNotLock, false);
        }
    }
}

void AMatch_PlayerController::CreateSelectArmyWidget_Implementation(bool bDestroy)
{
    /* If this controller belongs to this machine... */
    if (IsLocalPlayerController())
    {
        /* If this widget is getting destroyed and it exists... */
        if (bDestroy && MatchSetup_SelectArmy)
        {
            /* Destroy this widget. */
            MatchSetup_SelectArmy->RemoveFromParent();
        }
        /* If this widget needs to be created... */
        else
        {
            /* If the army selection widget class is set... */
            if (IsValid(MatchSetup_SelectArmyClass))
            {
                /* Create and store the army selection widget for this player. */
                MatchSetup_SelectArmy = CreateWidget<UMatchSetup_SelectArmy>(GetWorld(), MatchSetup_SelectArmyClass, FName("Army Selection Widget"));

                /* If the widget was created successfully... */
                if (MatchSetup_SelectArmy)
                {
                    /* Add this widget to the viewport. */
                    MatchSetup_SelectArmy->AddToViewport(0);
                }
            }
        }
    }
}

void AMatch_PlayerController::CreatePlacePiecesWidget_Implementation(FArmyPresetStruct SelectedArmy, bool bDestroy)
{
    /* If this controller belongs to this machine... */
    if (IsLocalPlayerController())
    {
        /* If this widget is getting destroyed... */
        if (bDestroy)
        {
            /* If this widget exists... */
            if (IsValid(MatchSetup_PlacePieces))
            {
                /* Destroy this widget. */
                MatchSetup_PlacePieces->RemoveFromParent();
            }
        }
        /* If this widget needs to be created... */
        else
        {
            /* If the place pieces widget class is set... */
            if (IsValid(MatchSetup_PlacePiecesClass))
            {
                /* Create and store the place pieces widget for this player. */
                MatchSetup_PlacePieces = CreateWidget<UMatchSetup_PlacePieces>(GetWorld(), MatchSetup_PlacePiecesClass, FName("Place Pieces Widget"));

                /* If the widget was created successfully... */
                if (MatchSetup_PlacePieces)
                {
                    /* Add this widget to the viewport. */
                    MatchSetup_PlacePieces->AddToViewport(0);

                    /* Load the pieces of the player's army into the widget. */
                    MatchSetup_PlacePieces->LoadArmy(SelectedArmy);
                }
            }
        }
    }
}

void AMatch_PlayerController::CreateBaseWidget_Implementation(bool bDestroy, bool bHide)
{
    /* If this controller belongs to this machine... */
    if (IsLocalPlayerController())
    {
        /* If the base widget was already created. */
        if (Match_BaseWidget)
        {
            /* If this widget needs to be destroyed... */
            if (bDestroy)
            {
                /* Destroy it. */
                Match_BaseWidget->RemoveFromParent();
            }
            /* If this widget needs to be hidden... */
            else if (bHide)
            {
                /* Hide this widget and its children. */
                Match_BaseWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
            /* If this widget needs to be reveled. */
            else
            {
                /* Reveal this widget and its children. */
                Match_BaseWidget->SetVisibility(ESlateVisibility::Visible);
            }
        }
        /* If the widget needs to be created... */
        else
        {
            /* If the base widget class is set... */
            if (IsValid(Match_BaseWidgetClass))
            {
                /* Create and store the base widget for this player. */
                Match_BaseWidget = CreateWidget<UMatch_BaseWidget>(GetWorld(), Match_BaseWidgetClass, FName("Base Widget"));

                /* If the widget was created successfully... */
                if (Match_BaseWidget)
                {
                    /* Add this widget to the viewport. */
                    Match_BaseWidget->AddToViewport(0);

                    /* Update the displayed map name with the game state, not a passed name. */
                    UpdateMapName("");
                }
            }
        }
    }
}

void AMatch_PlayerController::UpdateTimer()
{
    /* Only execute on local client, if the base widget is initialized, and if the game state class can be found. */
    if (IsLocalPlayerController() && Match_BaseWidget && GetWorld()->GetGameState<AMatch_GameStateBase>())
    {
        /* Get the current match time from the game state and display it on the base widget. */
        Match_BaseWidget->UpdateTimer(GetWorld()->GetGameState<AMatch_GameStateBase>()->MatchTime);
    }
}

void AMatch_PlayerController::UpdateMapName(FName NewName)
{
    /* Only execute on local client, if the base widget is initialized, and if the game state class can be found. */
    if (IsLocalPlayerController() && Match_BaseWidget && GetWorld()->GetGameState<AMatch_GameStateBase>())
    {
        /* Get the map name from the game state and display it on the base widget. */
        Match_BaseWidget->UpdateMapName(GetWorld()->GetGameState<AMatch_GameStateBase>()->MapName);
    }
}

void AMatch_PlayerController::UpdatePieceInfoWidget(AParentPiece *NewPiece, bool bIsFriendly, bool bEnableButtons, bool bHide)
{
    /* Only execute on local client. I added this check a long time ago but I think it's actually redundant... I'm scared to remove it though. */
    if (IsLocalPlayerController())
    {
        /* Make sure that the base widget has been created. */
        if (Match_BaseWidget)
        {
           /* If one of the piece info widgets needs to be hidden... */
            if (bHide)
            {
                /* If the piece info widget is currently visible... */
                if (Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->GetVisibility() == ESlateVisibility::Visible)
                {
                    /* Disable the widget and play the closing animation, which will hide the widget after playing. */
                    Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->SetVisibility(ESlateVisibility::HitTestInvisible);
                    Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->PlayOpenCloseAnim(false, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
                }
            }
            /* If a piece info widget needs to be revealed and the piece info widget class is set... */
            else
            {
                /* Update the widget's displayed information, enable it, and determine whether or not any information changed. */
                const bool bInfoChanged = Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->UpdatePieceInfoWidget(NewPiece, bIsFriendly, bEnableButtons);
                Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->SetVisibility(ESlateVisibility::HitTestInvisible);

                /* If any information changed or the widget wasn't already revealed, play the opening animation. */
                if (bInfoChanged || Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->GetRenderOpacity() < 1.0f)
                    Match_BaseWidget->GetPieceInfoWidget(bIsFriendly)->PlayOpenCloseAnim(true, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
            }
        }
    }
}

void AMatch_PlayerController::UpdateMoveConfirmationWidget(bool bDestroy, ABoardTile *PendingTile, AParentPiece *PendingPiece)
{
    /* Only execute on local client. */
    if (IsLocalPlayerController())
    {
        /* If the confirmation widget needs to be destroyed and the widget exists... */
        if (bDestroy && Match_MoveConfirmation)
        {
            /* Remove the widget as if the "cancel" button were pressed. */
            Match_MoveConfirmation->OnCancelClicked();
        }
        /* If the confirmation widget needs to be created... */
        else if (!bDestroy)
        {
            /* Create the widget. */
            Match_MoveConfirmation = CreateWidget<UMatch_MoveConfirmation>(GetWorld(), Match_MoveConfirmationClass, FName("Move Confirmation Widget"));

            /* If the widget was created successfully... */
            if (Match_MoveConfirmation)
            {
                /* Update the widget's pending tile and piece. */
                Match_MoveConfirmation->SetPendingTile(PendingTile);
                Match_MoveConfirmation->SetPendingPiece(PendingPiece);

                /* Add this widget to the viewport. */
                Match_MoveConfirmation->AddToViewport(0);
            }
        }
    }
}

void AMatch_PlayerController::UpdateAttackConfirmationWidget(bool bDestroy, AParentPiece* FriendlyPiece,
    AParentPiece* EnemyPiece)
{
    /* Only execute on local client. */
    if (IsLocalPlayerController())
    {
        /* If the attack confirmation widget needs to be destroyed and it's already been created... */
        if (bDestroy && Match_AttackConfirmation)
        {
            /* Remove the widget as if the "cancel" button were pressed. */
            Match_AttackConfirmation->OnCancelClicked();
        }
        /* If the widget needs to be created... */
        else if (!bDestroy)
        {
            /* Create the widget. */
            Match_AttackConfirmation = CreateWidget<UMatch_AttackConfirmation>(GetWorld(), Match_AttackConfirmationClass, FName("Attack Confirmation Widget"));

            /* If the attack confirmation widget was successfully created... */
            if (Match_AttackConfirmation)
            {
                /* Update the widget's displayed information using the given pieces. */
                Match_AttackConfirmation->UpdateAttackPreviewInfo(FriendlyPiece, EnemyPiece);

                /* Add the widget to the player's viewport. */
                Match_AttackConfirmation->AddToViewport(0);
            }
        }
    }
}

void AMatch_PlayerController::UpdateAttackGraphicWidget(bool bDestroy, AParentPiece* Attacker,
    AParentPiece* Defender)
{
    /* Only execute on local client. */
    if (IsLocalPlayerController())
    {
        /* If the attack graphic widget needs to be destroyed and it's already been created... */
        if (bDestroy && Match_AttackGraphic)
        {
            /* Remove the widget, destructing it. */
            Match_AttackGraphic->RemoveFromParent();
        }
        /* If the widget needs to be created... */
        else if (!bDestroy)
        {
            /* Create the widget. */
            Match_AttackGraphic = CreateWidget<UMatch_AttackGraphic>(GetWorld(), Match_AttackGraphicClass,
                FName("Attack Graphic Widget"));

            /* If the attack graphic widget was successfully created... */
            if (Match_AttackGraphic)
            {
                /* Update the widget's displayed information using the given pieces. */
                Match_AttackGraphic->UpdateAttackGraphicInfo(Attacker, Defender);

                /* Add the widget to the player's viewport. */
                Match_AttackGraphic->AddToViewport(0);
            }
        }
    }
}

void AMatch_PlayerController::PlayAttackGraphicAnimation(EAttackGraphicAnimation AttackGraphicAnim)
{
    /* Only execute on local client. */
    if (IsLocalPlayerController())
    {
        /* If the attack graphic widget currently exists (i.e. is on the screen). */
        if (Match_AttackGraphic)
        {
            /* Play the given animation on the widget. */
            Match_AttackGraphic->PlayAttackGraphicAnimation(AttackGraphicAnim);
        }
    }
}

void AMatch_PlayerController::UpdateActiveAbilityConfirmationWidget(bool bDestroy, AParentPiece* AbilityUser, AActor* Target)
{
    /* Only execute on local client. */
    if (IsLocalPlayerController())
    {
        /* If the active ability confirmation widget needs to be destroyed and it's already been created... */
        if (bDestroy && Match_ActiveAbilityConfirmation)
        {
            /* Remove the widget as if the "cancel" button were pressed. */
            Match_ActiveAbilityConfirmation->OnCancelClicked();
        }
        /* If the widget needs to be created... */
        else if (!bDestroy)
        {
            /* Create the widget. */
            Match_ActiveAbilityConfirmation = CreateWidget<UMatch_ActiveAbilityConfirmation>(GetWorld(), Match_ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));

            /* If the active ability confirmation widget was successfully created... */
            if (Match_ActiveAbilityConfirmation)
            {
                /* Update the widget's information using the given piece. This can be expanded upon in the future
                 * if the confirmation widget needs to display more information. */
                Match_ActiveAbilityConfirmation->UpdateActionConfirmationInfo(AbilityUser, Target);

                /* Add the widget to the player's viewport. */
                Match_ActiveAbilityConfirmation->AddToViewport(0);
            }
        }
    }
}