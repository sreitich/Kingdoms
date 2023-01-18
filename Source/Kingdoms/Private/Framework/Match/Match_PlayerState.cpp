// Copyright Change Studios, LLC 2023.


#include "Framework/Match/Match_PlayerState.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Kismet/GameplayStatics.h"

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

void AMatch_PlayerState::Server_EndPlayerTurn_Implementation()
{
    /* End this player's turn and start the next player's turn. */
    if (AMatch_GameStateBase* GameStatePtr = Cast<AMatch_GameStateBase>(UGameplayStatics::GetGameState(this)))
    {
        GameStatePtr->Server_SwitchTurn(this);
    }
}

void AMatch_PlayerState::SetReadyToPlay(bool bReady)
{
    /* Locally update the player's "ready" state to reduce delay. */
    bReadyToPlay = bReady;

    /* OnRep functions are not called automatically locally. */
    OnRep_bReadyToPlay();

    /* If this was called on a non-server client, update the player's "ready" state on the server. */
    if (!HasAuthority())
    {
        Server_SetReadyToPlay(bReady);
    }
}

void AMatch_PlayerState::SetPlayerStatus(EPlayerStatus NewPlayerStatus)
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

    /* Save the player's old player status for logic that executes based on transitions. */
    const EPlayerStatus OldPlayerStatus = CurrentPlayerStatus;

    /* Locally update the player's status to reduce delay. */
    CurrentPlayerStatus = NewPlayerStatus;

    /* OnRep functions are not called automatically locally. */
    OnRep_CurrentPlayerStatus(OldPlayerStatus);

    /* If this was called on a non-server client, update the player's status on the server. */
    if (!HasAuthority())
    {
        Server_SetPlayerStatus(NewPlayerStatus);
    }
}

void AMatch_PlayerState::SetMoveActionUsed()
{
    /* Players' action uses start as false by default and are only changed by updating their state. This function can
     * only be used to prevent players from taking more move actions, if the player is in a valid state to have used one. */
    if (CurrentPlayerStatus == E_SelectingPiece ||
        CurrentPlayerStatus == E_SelectingAction ||
        CurrentPlayerStatus == E_SelectingTarget_Move ||
        CurrentPlayerStatus == E_SelectingTarget_ActiveAbility)
    {
        /* Locally update the move action usage to reduce delay. */
        bMoveActionUsed = true;

        /* OnRep functions are not called automatically locally. */
        OnRep_MoveActionUsed();

        /* If this was called on a non-server client, update the move action usage on the server. */
        if (!HasAuthority())
        {
            Server_SetMoveActionUsed();
        }
    }
}

void AMatch_PlayerState::SetAbilityActionUsed()
{
    /* Players' action uses start as false by default and are only changed by updating their state. This function can
     * only be used to prevent players from taking more ability actions, if the player is in a valid state to have used one. */
    if (CurrentPlayerStatus == E_SelectingPiece ||
        CurrentPlayerStatus == E_SelectingAction ||
        CurrentPlayerStatus == E_SelectingTarget_Move ||
        CurrentPlayerStatus == E_SelectingTarget_ActiveAbility)
    {

        /* Locally update the ability action usage to reduce delay. */
        bAbilityActionUsed = true;

        /* OnRep functions are not called automatically locally. */
        OnRep_AbilityActionUsed();

        /* If this was called on a non-server client, update the ability action usage on the server. */
        if (!HasAuthority())
        {
            Server_SetAbilityActionUsed();
        }
    }
}

void AMatch_PlayerState::Client_SetIsInSequence_Implementation(bool bNewIsInSequence)
{
    /* Update whether this player is in a sequence. */
    SetIsInSequence(bNewIsInSequence);
}

void AMatch_PlayerState::SetIsInSequence(bool bNewIsInSequence)
{
    /* Update whether this player is in a sequence. */
    bIsInSequence = bNewIsInSequence;

    /* Refresh this player's piece info widgets to enable or disable the buttons, depending on the new value of
     * bIsInSequence. */
    GetPawn<AMatch_PlayerPawn>()->RefreshPieceInfoWidgets();

    /* Enable or disable this player's "end turn" button. */
    const AMatch_PlayerController* PlayerControllerPtr = GetPawn()->GetController<AMatch_PlayerController>();
    if (IsValid(PlayerControllerPtr))
    {
        /* Disable the "end turn" button if this player is now in a sequence. */
        if (bIsInSequence)
        {
            PlayerControllerPtr->UpdateEndTurnButton(false);
        }
        /* Enable the "end turn" button if this player is no longer in a sequence and has already used their move
         * action. */
        else
        {
            PlayerControllerPtr->UpdateEndTurnButton(bMoveActionUsed);
        }
    }
}

void AMatch_PlayerState::OnRep_bReadyToPlay()
{
    /* If this is the server and all players are ready to start, begin the match. */
    if (HasAuthority())
    {
        AMatch_GameStateBase* MatchGameState = GetWorld()->GetGameState<AMatch_GameStateBase>();
        if (MatchGameState && MatchGameState->CheckReadyToStart())
            MatchGameState->Server_StartMatch();
    }
}

void AMatch_PlayerState::OnRep_CurrentPlayerStatus(EPlayerStatus OldPlayerStatus)
{
    /* If the player was waiting for their turn and is now selecting a piece, reset their turn progress and check if
     * they already have a piece info widget open. */
    if (CurrentPlayerStatus == E_SelectingPiece && OldPlayerStatus == E_WaitingForTurn)
    {
        bMoveActionUsed = false;
        bAbilityActionUsed = false;

        /* Manually call OnReps if this is the server. */
        if (HasAuthority())
        {
            OnRep_MoveActionUsed();
            OnRep_AbilityActionUsed();
        }

        /* If this player has a friendly piece info widget open when it becomes their turn, set their current status
         * to E_SelectingAction instead of E_SelectingPiece. */
        if (const AMatch_PlayerController* ControllerPtr = GetPawn()->GetController<AMatch_PlayerController>())
        {
            if (ControllerPtr->IsPieceInfoWidgetOpen(E_Friendly))
            {
                SetPlayerStatus(E_SelectingAction);
            }
        }

        /* Refresh this player's piece info widgets when their turn starts and their player status has been updated. */
        GetPawn<AMatch_PlayerPawn>()->Client_RefreshPieceInfoWidgets();
    }
    /* When the player's turn ends, refresh their piece info widgets. */
    else if (CurrentPlayerStatus == E_WaitingForTurn)
    {
        /* Refresh this player's piece info widgets after their player status has been updated. */
        GetPawn<AMatch_PlayerPawn>()->Client_RefreshPieceInfoWidgets();
    }
}

void AMatch_PlayerState::OnRep_MoveActionUsed()
{
    /* Update the turn progress indicators to indicate whether the move action has been used. */
    if (const AMatch_PlayerController* PlayerControllerPtr = GetPawn()->GetController<AMatch_PlayerController>())
    {
        PlayerControllerPtr->UpdateActionIndicator(bMoveActionUsed, true);
    }

    /* Enable the end turn button to allow the player to end their turn after using a move action. */
    if (bMoveActionUsed)
    {
        if (const AMatch_PlayerController* PlayerControllerPtr = GetPawn()->GetController<AMatch_PlayerController>())
        {
            // PlayerControllerPtr->UpdateEndTurnButton(true);
        }
    }
}

void AMatch_PlayerState::OnRep_AbilityActionUsed()
{
    /* Update the turn progress indicators to indicate whether the ability action has been used. */
    if (const AMatch_PlayerController* PlayerControllerPtr = GetPawn()->GetController<AMatch_PlayerController>())
        PlayerControllerPtr->UpdateActionIndicator(bAbilityActionUsed, false);
}

void AMatch_PlayerState::Server_SetReadyToPlay_Implementation(bool bReady)
{
    /* Set bReadyToPlay on the server. */
    bReadyToPlay = bReady;

    /* The server does not call the OnRep automatically. */
    OnRep_bReadyToPlay();
}

void AMatch_PlayerState::Server_SetPlayerStatus_Implementation(EPlayerStatus NewPlayerStatus)
{
    /* Save what the original player status was. */
    const EPlayerStatus OldPlayerStatus = CurrentPlayerStatus;

    /* Update the player status on the server. */
    CurrentPlayerStatus = NewPlayerStatus;

    /* The server does not call the OnRep automatically. */
    OnRep_CurrentPlayerStatus(OldPlayerStatus);
}

void AMatch_PlayerState::Server_SetMoveActionUsed_Implementation()
{
    /* Set the move action usage on the server. */
    bMoveActionUsed = true;
    
    /* The server does not call the OnRep automatically. */
    OnRep_MoveActionUsed();
}

void AMatch_PlayerState::Server_SetAbilityActionUsed_Implementation()
{
    /* Set the ability action usage on the server. */
    bAbilityActionUsed = true;

    /* The server does not call the OnRep automatically. */
    OnRep_AbilityActionUsed();
}