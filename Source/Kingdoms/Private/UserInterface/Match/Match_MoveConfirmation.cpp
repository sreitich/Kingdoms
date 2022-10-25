// Copyright Samuel Reitich 2022.


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
    /* Reset the highlight of every tile that was highlighted. */
    for (ABoardTile* Tile : PendingPiece->GetValidMoveTiles())
    {
        Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
    }
    
    /* Remove all other widgets and reset the player's selected piece and selected tile when this piece starts moving. */
    GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true, false, false, true);

    /* Reset this piece's rotation after it finishes moving. */
    Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_SetResetAfterMove(PendingPiece, true);

    /* Move the piece to the tile on the server via the player controller's server communication component. */
    Cast<AMatch_PlayerController>(PendingPiece->GetInstigator()->GetController())->
        GetServerCommunicationComponent()->Server_MovePieceToTile(PendingPiece, PendingTile, true);

    /* Destroy this widget. */
    RemoveFromParent();
}

void UMatch_MoveConfirmation::OnCancelClicked()
{
    /* Reset the player state. */
    GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
    /* Reset the player's selected piece and selected tile. */
    GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true, false, false, true);

    /* Reset the highlight of every tile that was highlighted. */
    if (IsValid(PendingPiece))
    {
        for (ABoardTile* Tile : PendingPiece->GetValidMoveTiles())
        {
            Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
        }
    }

    /* Destroy this widget. */
    RemoveFromParent();
}
