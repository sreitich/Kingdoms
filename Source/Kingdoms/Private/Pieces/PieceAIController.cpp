// Copyright Samuel Reitich 2022.


#include "Pieces/PieceAIController.h"

#include "Board/BoardTile.h"

APieceAIController::APieceAIController()
{

}

void APieceAIController::MovePieceToTile(ABoardTile* NewTile)
{
    /* If the passed new tile isn't null and if this is being called by the piece's owner... */
    if (IsValid(NewTile) && HasAuthority())
    {
        /* Move the piece to that tile's location. */
        MoveToLocation
        (
            /* Destination as a vector. The root of the tile and the root of the piece are 165 units away: tile height (150) / 2 + 90). */
            FVector(NewTile->GetActorLocation().X, NewTile->GetActorLocation().Y, NewTile->GetActorLocation().Z + 165.0f),
            /* Acceptance radius: pawn will stop moving when it gets this close to the location. */
            0.1f,
            /* Stop when the pawn overlaps with the location. */
            false,
            /* Whether or not to use pathfinding. If false, pawn will always go in a straight line. */
            true,
            /* Whether or not to project the location on navigation data before using it. */
            false,
            /* Whether or not to strafe (sets bAllowStrafe). */
            false,
            /* Whether or not to use an incomplete path when the target location can't be reached. */
            nullptr
        );
    }
}