// Copyright Samuel Reitich 2022.


#include "Framework/Match/Match_GameStateBase.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Components/PieceNetworkingComponent.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "UserDefinedData/SaveGameData_UserDefinedData.h"
#include "Pieces/ParentPiece.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#define OUT

AMatch_GameStateBase::AMatch_GameStateBase()
{
    /* Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = false;

    /* Enable replication on this actor. */
    bReplicates = true;

}

void AMatch_GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /* Replicate these variables. */
    DOREPLIFETIME(AMatch_GameStateBase, BoardManager);
    DOREPLIFETIME(AMatch_GameStateBase, CurrentMatchStatus);
    DOREPLIFETIME(AMatch_GameStateBase, MatchTime);
    DOREPLIFETIME(AMatch_GameStateBase, TurnTime);
    DOREPLIFETIME(AMatch_GameStateBase, bAnimatedPiece);
    DOREPLIFETIME(AMatch_GameStateBase, PlayerStarts);
    DOREPLIFETIME(AMatch_GameStateBase, CurrentAttackInfo);
}

void AMatch_GameStateBase::SetUpMatch()
{
    /* For every player state... */
    for (APlayerState* PlayerState : PlayerArray)
    {
        /* If this is the correct player state class... */
        if (Cast<AMatch_PlayerState>(PlayerState))
        {
            /* Set their player state to "placing pieces." */
            Cast<AMatch_PlayerState>(PlayerState)->Server_SetPlayerStatus(E_PlacingPieces);

            /* Cast to their controller to create the army selection widget. */
            PlayerState->GetPawn<AMatch_PlayerPawn>()->GetController<AMatch_PlayerController>()->CreateSelectArmyWidget(false);
        }
    }
}

bool AMatch_GameStateBase::CheckReadyToStart()
{
    /* Don't try to start the game if it has already been started. */
    if (bMatchStarted)
        return false;
    
    /* For every player in the PlayerArray... */
    for (APlayerState* PlayerState : PlayerArray)
    {
        /* If this is player has the correct player state class (i.e. not a spectator)... */
        if (Cast<AMatch_PlayerState>(PlayerState))
        {
            /* If the player isn't ready... */
            if (!Cast<AMatch_PlayerState>(PlayerState)->GetReadyToPlay())
            {
                /* The match is not ready to start. */
                return false;
            }
        }
    }

    /* If every player is ready to start the match, the match can begin. */
    return true;
}

void AMatch_GameStateBase::Server_EndTurn_Implementation(AMatch_PlayerState* CurrentPlayer)
{
    /* Get the player whose turn we're switching to. */
    AMatch_PlayerState* NewPlayer = nullptr;
    for (APlayerState* PlayerStatePtr : PlayerArray)
    {
        if (PlayerStatePtr != CurrentPlayer)
        {
            NewPlayer = Cast<AMatch_PlayerState>(PlayerStatePtr);
        }
    }

    /* End the last player's turn, which will reset their used actions and action indicators. */
    if (IsValid(CurrentPlayer))
        CurrentPlayer->Server_SetPlayerStatus(E_WaitingForTurn);

    /* Decrement the modifier durations for the player whose turn just ended. */
    Server_DecrementModifierDurations(CurrentPlayer);

    /* Start the next player's turn. */
    if (IsValid(NewPlayer))
        NewPlayer->Server_SetPlayerStatus(E_SelectingPiece);
}

void AMatch_GameStateBase::Server_StartMatch_Implementation()
{
    /* Prevent the match from starting again. */
    bMatchStarted = true;


    /* Reveal all pieces for all players. */
    Multicast_RevealAllPieces();


    /* Call piece-specific game-start code on every piece in the game. */
    TArray<AActor*> AllPieces;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AParentPiece::StaticClass(), OUT AllPieces);
    for (AActor* Piece : AllPieces)
        Cast<AParentPiece>(Piece)->OnGameStart();


    /* For every player state... */
    for (APlayerState* PlayerState : PlayerArray)
    {
        /* If this is the correct player state class and player controller class... */
        if (Cast<AMatch_PlayerState>(PlayerState) && PlayerState->GetPawn()->GetController<AMatch_PlayerController>())
        {
            /* Cast to their controller to destroy the place pieces widget and create the base widget. */
            PlayerState->GetPawn()->GetController<AMatch_PlayerController>()->CreatePlacePiecesWidget(FArmyPresetStruct(), true);
            PlayerState->GetPawn()->GetController<AMatch_PlayerController>()->CreateBaseWidget(false, false);

            /* If this is player 1... */
            if (Cast<AMatch_PlayerState>(PlayerState)->PlayerIndex == 1)
            {
                /* Set their player state to "selecting piece." */
                Cast<AMatch_PlayerState>(PlayerState)->Server_SetPlayerStatus(E_SelectingPiece);
            }
            /* If this is player 2... */
            else if (Cast<AMatch_PlayerState>(PlayerState)->PlayerIndex == 2)
            {
                /* Set their player state to "waiting for turn." */
                Cast<AMatch_PlayerState>(PlayerState)->Server_SetPlayerStatus(E_WaitingForTurn);
            }
        }
    }

    /* Update the match status. */
    SetMatchStatus(E_Player1Turn);

    /* Begin the timer by setting the Count function to call every second. */
    GetWorld()->GetTimerManager().SetTimer(MatchTimeTimerHandle, this, &AMatch_GameStateBase::Count_Implementation, 1.0f, true);
}

void AMatch_GameStateBase::BeginPlay()
{
    Super::BeginPlay();

    /* Set the board manager pointer so at the beginning of the game so that no one has to look for it each time they
     * need to use it. */
    TArray<AActor*> BoardManagersArray;
    UGameplayStatics::GetAllActorsOfClass(this, ABoardManager::StaticClass(), OUT BoardManagersArray);
    if (BoardManagersArray.Num() > 0)
        BoardManager = Cast<ABoardManager>(BoardManagersArray[0]);
}

void AMatch_GameStateBase::OnRep_CurrentMatchStatus()
{
}

void AMatch_GameStateBase::Multicast_RevealAllPieces_Implementation()
{
    /* Get every piece on the board. */
    TArray<AActor*> AllPieces;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AParentPiece::StaticClass(), OUT AllPieces);

    for (AActor* Piece : AllPieces)
    {
        /* Unhide each piece so that players can now see each other's pieces. */
        Piece->SetActorHiddenInGame(false);
    }
}

void AMatch_GameStateBase::Server_DecrementModifierDurations_Implementation(AMatch_PlayerState* OwningPlayer)
{
    /* Get every piece in the game. */
    TArray<AActor*> AllPieceActors;
    UGameplayStatics::GetAllActorsOfClass(this, AParentPiece::StaticClass(), OUT AllPieceActors);

    /* Iterate through every piece. */
    for (AActor* PieceActor : AllPieceActors)
    {
        /* Only decrement the modifier durations of the given player's pieces. */
        if (PieceActor->GetInstigator() == OwningPlayer->GetPawn())
        {
            /* Get the piece actor as a piece. */
            if (AParentPiece* Piece = Cast<AParentPiece>(PieceActor))
            {
                /* For every modifier applied to this piece... */
                for (int i = 0; i < Piece->GetTemporaryModifiers().Num(); i++)
                {
                    /* Check if the iterated modifier has an indefinite duration. These modifiers' durations don't get decremented. */
                    if (!Piece->GetTemporaryModifiers()[i].bIndefiniteDuration)
                    {
                        /* Reduce the modifier's remaining duration by 1 turn. */
                        const int NewRemainingDuration = Piece->GetTemporaryModifiers()[i].RemainingDuration - 1;

                        /* If the modifier's duration has now ended, remove it. */
                        if (NewRemainingDuration < 1)
                        {
                            /* Always activate pop-ups for abilities that are removed as a result of their duration ending. */
                            Cast<AMatch_PlayerPawn>(Piece->GetInstigator())->GetPieceNetworkingComponent()->Server_RemoveModifier(Piece, Piece->GetTemporaryModifiers()[i], true, true);
                        }
                    }
                }
            }
        }
    }
}

void AMatch_GameStateBase::SetMatchStatus_Implementation(EMatchStatus NewMatchStatus)
{
    /* Ways match states can change:
     * - Waiting for players
     *      - Nothing
     * - Setting up pieces
     *      - From Waiting for players
     * - Player 1 turn
     *      - Setting up pieces
     *      - Player 2 turn
     * - Player 2 turn
     *      - Player 1 turn
     * - Ending game
     *      - Setting up pieces
     *      - Player 1 turn
     *      - Player 2 turn
     */
    
    CurrentMatchStatus = NewMatchStatus;
}

void AMatch_GameStateBase::Count_Implementation()
{
    /* Increment the match time every second. MatchTime is taken by each player controller to display on the timer widget each tick. */
    MatchTime++;
}