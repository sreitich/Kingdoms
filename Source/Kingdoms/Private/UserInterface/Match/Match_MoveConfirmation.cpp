// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_MoveConfirmation.h"

#include "Board/BoardTile.h"
#include "Pieces/ParentPiece.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_PlayerState.h"

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
    /* Refresh the highlights of every highlighted tile before the server updates the piece's location. GetValidTiles()
     * won't work properly if called afterwards. */
    for (ABoardTile* Tile : PendingPiece->GetValidTiles())
    {
        Tile->RefreshHighlight();
    }
    
    /* Move the piece to the tile on the server via the player controller's server communication component. */
    Cast<AMatch_PlayerController>(PendingPiece->GetInstigator()->GetController())->
        GetServerCommunicationComponent()->MovePieceToTile_Server(PendingPiece, PendingTile, true);

    /* Destroy this widget. */
    RemoveFromParent();
}

void UMatch_MoveConfirmation::OnCancelClicked()
{
    /* Reset the player state. */
    GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);

    /* For every tile that was highlighted... */
    for (ABoardTile* Tile : PendingPiece->GetValidTiles())
    {
        /* Refresh the tile's highlight depending on its occupying piece to clear the highlights. */
        Tile->RefreshHighlight();
    }

    /* Destroy this widget. */
    RemoveFromParent();
}