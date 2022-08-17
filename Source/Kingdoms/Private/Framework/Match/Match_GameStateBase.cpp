// Copyright Samuel Reitich 2022.


#include "Framework/Match/Match_GameStateBase.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
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
            Cast<AMatch_PlayerState>(PlayerState)->SetPlayerStatus_Server(E_PlacingPieces);

            /* Cast to their controller to create the army selection widget. */
            PlayerState->GetPawn<AMatch_PlayerPawn>()->GetController<AMatch_PlayerController>()->CreateSelectArmyWidget(false);
        }
    }
}

bool AMatch_GameStateBase::CheckReadyToStart()
{
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

void AMatch_GameStateBase::StartMatch()
{
    /* Get every piece on the board. */
    TArray<AActor*> AllPieces;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AParentPiece::StaticClass(), OUT AllPieces);

    /* Unhide each piece so that players can now see each other's pieces. */
    for (AActor* Piece : AllPieces)
    {
        Piece->SetActorHiddenInGame(false);
    }
    
    /* Refresh each tile's highlight so that players see each other's piece highlights. */
    if (IsValid(BoardManager))
    {
        BoardManager->RefreshBoard();
    }

    
    /* Only allow the server to begin the game. */
    if (HasAuthority())
    {
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
                    Cast<AMatch_PlayerState>(PlayerState)->SetPlayerStatus_Server(E_SelectingPiece);
                }
                /* If this is player 2... */
                else if (Cast<AMatch_PlayerState>(PlayerState)->PlayerIndex == 2)
                {
                    /* Set their player state to "waiting for turn." */
                    Cast<AMatch_PlayerState>(PlayerState)->SetPlayerStatus_Server(E_WaitingForTurn);
                }
            }
        }

        /* Update the match status. */
        SetMatchStatus(E_Player1Turn);

        /* Begin the timer by setting the Count function to call every second. */
        GetWorld()->GetTimerManager().SetTimer(MatchTimeTimerHandle, this, &AMatch_GameStateBase::Count_Implementation, 1.0f, true);
    }
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