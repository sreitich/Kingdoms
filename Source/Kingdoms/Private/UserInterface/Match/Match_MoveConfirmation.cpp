// Copyright Change Studios, LLC 2023.


#include "UserInterface/Match/Match_MoveConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/RectLightComponent.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/UMG/Public/UMG.h"

bool UMatch_MoveConfirmation::SetPendingTile(ABoardTile* NewPendingTile)
{
    /* If the passed tile is valid... */
    if (IsValid(NewPendingTile))
    {
        /* Set the pending tile to the passed tile. */
        PendingTile = NewPendingTile;

        return true;
    }

    return false;
}

bool UMatch_MoveConfirmation::SetPendingPiece(AParentPiece* NewPendingPiece)
{
    /* If the passed piece is valid... */
    if (IsValid(NewPendingPiece))
    {
        /* Set the pending piece to the passed piece. */
        PendingPiece = NewPendingPiece;

        return true;
    }

    return false;
}

void UMatch_MoveConfirmation::SetConfirmButtonIsEnabled(bool bConfirmIsEnabled)
{
    /* Enable/disable the confirmation button. */
    ConfirmButton->SetIsEnabled(bConfirmIsEnabled);
}

void UMatch_MoveConfirmation::DestroyWidget(bool bReset)
{
    AMatch_PlayerPawn* PlayerPawnPtr = GetOwningPlayerPawn<AMatch_PlayerPawn>();
    
    /* Reset the player's state and highlighted tiles if requested. */
    if (bReset)
    {
        /* Reset the player state. */
        PlayerPawnPtr->GetPlayerState<AMatch_PlayerState>()->SetPlayerStatus(E_SelectingPiece);

        /* Reset the highlight of every tile that was highlighted. */
        if (IsValid(PendingPiece))
        {
            for (ABoardTile* Tile : PendingPiece->GetValidMoveTiles())
            {
                Tile->RemoveTargetHighlight();
            }
        }

        /* Reset the player's selected piece and selected tile and hide the piece info widgets. */
        PlayerPawnPtr->ClearSelection(true, false, false, true, true);
    }
    /* Just reset the player's selected tile if bReset is false. This is typically only used when switching to an attack widget. */
    else
    {
        PlayerPawnPtr->ClearSelection(false, false, false, true, false);
    }

    /* Clear this widget's information and disable the confirmation button. */
    PendingPiece = nullptr;
    PendingTile = nullptr;
    ConfirmButton->SetIsEnabled(false);

    /* Destroy this widget. */
    RemoveFromParent();
}

void UMatch_MoveConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

    /* Bind the confirm button to move the selected piece to the selected tile. */
    ConfirmButton->OnClicked.AddDynamic(this, &UMatch_MoveConfirmation::OnConfirmClicked);
    /* Bind the cancel button to reset the move action. */
    CancelButton->OnClicked.AddDynamic(this, &UMatch_MoveConfirmation::OnCancelClicked);
}

void UMatch_MoveConfirmation::OnConfirmClicked()
{
    /* Get the local player's controller, player state, and pawn. */
    AMatch_PlayerController* PlayerControllerPtr = GetOwningPlayer<AMatch_PlayerController>();
    AMatch_PlayerState* PlayerStatePtr = GetOwningPlayerState<AMatch_PlayerState>();
    AMatch_PlayerPawn* PlayerPawnPtr = GetOwningPlayerPawn<AMatch_PlayerPawn>();
	if (PlayerControllerPtr && PlayerStatePtr && PlayerPawnPtr)
    {
        /* Record that the player has used their move action for this turn, preventing them from using another move action until their next turn. */
        PlayerStatePtr->SetMoveActionUsed();

		/* Set that the player is in a sequence to prevent them from taking actions or ending their turn until the sequence ends. */
	    PlayerStatePtr->SetIsInSequence(true);

        /* Reset this piece's rotation after it finishes moving. */
        PlayerPawnPtr->Server_SetResetAfterMove(PendingPiece, true);

        /* Move the piece to the tile on the server via the player controller's server communication component. */
        PlayerControllerPtr->GetServerCommunicationComponent()->Server_MovePieceToTile(PendingPiece, PendingTile, true);

        /* Destroy this widget through the player controller to clean up references. */
        PlayerControllerPtr->CreateMoveConfirmationWidget(true, nullptr);
    }
}

void UMatch_MoveConfirmation::OnCancelClicked()
{
    /* Destroy this widget through the player controller to clean up references. */
    if (AMatch_PlayerController* PlayerControllerPtr = Cast<AMatch_PlayerController>(GetOwningPlayer()))
        PlayerControllerPtr->CreateMoveConfirmationWidget(true, nullptr);
}
