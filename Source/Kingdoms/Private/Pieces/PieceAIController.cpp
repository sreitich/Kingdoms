// Copyright Samuel Reitich 2022.


#include "Pieces/PieceAIController.h"

#include "Board/BoardTile.h"
#include "Pieces/ParentPiece.h"

APieceAIController::APieceAIController()
{
}

void APieceAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    if (bResetAfterMove)
    {
        /* Reset the piece's rotation whenever it finishes moving. */
        Cast<AParentPiece>(GetPawn())->Server_ResetPieceRotation();
    }
}