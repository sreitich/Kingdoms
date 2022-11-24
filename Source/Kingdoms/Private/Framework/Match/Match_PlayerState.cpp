// Copyright Samuel Reitich 2022.


#include "Framework/Match/Match_PlayerState.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AMatch_PlayerState::AMatch_PlayerState()
{
    /* Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = false;

    /* Enable replication on this actor. */
    bReplicates = true;
}

void AMatch_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /* Replicate these variables. */
    DOREPLIFETIME(AMatch_PlayerState, PlayerIndex);
    DOREPLIFETIME(AMatch_PlayerState, CurrentPlayerStatus);
    DOREPLIFETIME(AMatch_PlayerState, OwnedPieces);
    DOREPLIFETIME(AMatch_PlayerState, bReadyToPlay);
    DOREPLIFETIME(AMatch_PlayerState, bMoveActionUsed);
    DOREPLIFETIME(AMatch_PlayerState, bAbilityActionUsed);
}

void AMatch_PlayerState::SetReadyToPlay_Server_Implementation(bool bReady)
{
    bReadyToPlay = bReady;

    /* Get the game state. */
    AMatch_GameStateBase* MatchGameState = GetWorld()->GetGameState<AMatch_GameStateBase>();

    /* If all players are ready to start... */
    if (MatchGameState && MatchGameState->CheckReadyToStart())
    {
        /* Start the match on each client with the server. */
        MatchGameState->Server_StartMatch();
    }
}

void AMatch_PlayerState::SetLocalPlayerStatus(EPlayerStatus NewPlayerStatus)
{
    CurrentPlayerStatus = NewPlayerStatus;
}

void AMatch_PlayerState::Server_SetMoveActionUsed_Implementation(bool bNewMoveActionUsed)
{
    /* Players' action uses start as false by default and are only changed by updating their state. This function can
     * only be used to prevent players from taking more move actions, if the player is in a valid state. */
    if (bNewMoveActionUsed &&
        (CurrentPlayerStatus == E_SelectingPiece ||
        CurrentPlayerStatus == E_SelectingAction ||
        CurrentPlayerStatus == E_SelectingTarget_Move ||
        CurrentPlayerStatus == E_SelectingTarget_ActiveAbility))
    {
        bMoveActionUsed = true;

        /* The server does not call the OnRep automatically. */
        OnRep_MoveActionUsed();
    }


}

void AMatch_PlayerState::Server_SetAbilityActionUsed_Implementation(bool bNewAbilityActionUsed)
{
    /* Players' action uses start as false by default and are only changed by updating their state. This function can
     * only be used to prevent players from taking more move actions, if the player is in a valid state. */
    if (bNewAbilityActionUsed &&
        (CurrentPlayerStatus == E_SelectingPiece ||
        CurrentPlayerStatus == E_SelectingAction ||
        CurrentPlayerStatus == E_SelectingTarget_Move ||
        CurrentPlayerStatus == E_SelectingTarget_ActiveAbility))
    {
        bAbilityActionUsed = true;

        /* The server does not call the OnRep automatically. */
        OnRep_AbilityActionUsed();
    }
}

void AMatch_PlayerState::Server_SetPlayerStatus_Implementation(EPlayerStatus NewPlayerStatus)
{
    /* Ways player states can change:
     * - Connecting
     *      - Nothing
     * - Placing pieces
     *      - From Connecting
     * - Waiting for turn
     *      - From Placing Pieces
     *      - From Selecting Piece
     *      - From Selecting Action
     *      - Maybe... (probably should make the player cancel their action before ending their turn)
         *      - From Selecting Target Move
         *      - From Selecting Target Active
         *      - From Selecting Target Passive
     * - Selecting Piece
     *      - From Placing Pieces
     *      - From Waiting for turn
     * - Selecting Action
     *      - From Selecting Piece
     * - Selecting Target Move
     *      - From Selecting Action
     * - Selecting Target Active
     *      - From Selecting Action
     * - Selecting Target Passive
     *      - From Placing Pieces
     *      - From Waiting for Turn
     *      - From Selecting Piece
     *      - From Selecting Action
     *      - From Selecting Target Move
     *      - From Selecting Target Active
     */

    const EPlayerStatus OldPlayerStatus = CurrentPlayerStatus;

    CurrentPlayerStatus = NewPlayerStatus;

    /* OnRep is not called on the server. */
    OnRep_CurrentPlayerStatus(OldPlayerStatus);
}

void AMatch_PlayerState::OnRep_CurrentPlayerStatus(EPlayerStatus OldPlayerStatus)
{
    /* If the player was waiting for their turn and they are now selecting a piece, reset their turn progress. */
    if (OldPlayerStatus == E_WaitingForTurn && CurrentPlayerStatus == E_SelectingPiece)
    {
        bMoveActionUsed = false;
        bAbilityActionUsed = false;
    }
}

void AMatch_PlayerState::OnRep_MoveActionUsed()
{
    UE_LOG(LogTemp, Error, TEXT("Move rep called."));
}

void AMatch_PlayerState::OnRep_AbilityActionUsed()
{
    UE_LOG(LogTemp, Error, TEXT("Ability rep called."));
}
