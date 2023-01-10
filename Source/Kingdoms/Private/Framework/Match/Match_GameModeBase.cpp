// Copyright Change Studios, LLC 2023.


#include "Framework/Match/Match_GameModeBase.h"

#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Framework/Match/Match_GameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameStateBase.h"

AMatch_GameModeBase::AMatch_GameModeBase()
{
 	/* Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = false;

}

void AMatch_GameModeBase::BeginPlay()
{
	Super::BeginPlay();


}

void AMatch_GameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* AMatch_GameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
    /* Get every player start in the game. */
    TArray<APlayerStart*> ClassArray;
    TArray<AActor*> ActorArray;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), OUT ActorArray);

    for (AActor* ActorIndex : ActorArray)
    {
        ClassArray.Add(Cast<APlayerStart>(ActorIndex));
    }

    /* If the passed controller is a match player and has a match player state... */
    if (Cast<AMatch_PlayerController>(Player) && Cast<AMatch_PlayerState>(Player->PlayerState))
    {
        /* Get pointers to this player controller and its player state. */
        AMatch_PlayerController* PlayerController = Cast<AMatch_PlayerController>(Player);
        AMatch_PlayerState* PlayerState = Cast<AMatch_PlayerState>(PlayerController->PlayerState);

        /* Get a casted game state reference to use throughout this function. */
        AMatch_GameStateBase* GameStatePtr = Cast<AMatch_GameStateBase>(UGameplayStatics::GetGameState(this));
        
        /* For every player start... */
        for (APlayerStart* PlayerStart : ClassArray)
        {
            /* If this is player 1... */
            if (GameStatePtr->PlayerArray.Num() == 1)
            {
                /* If the player start has the "player 1" tag... */
                if (PlayerStart->PlayerStartTag == "Player 1")
                {
                    /* Save this player start as player 1's for future reference. */
                    GameStatePtr->PlayerStarts.EmplaceAt(0, PlayerStart);

                    return PlayerStart;
                }
            }
            /* If this is player 2... */
            else
            {
                /* If the player start has the "player 2" tag... */
                if (PlayerStart->PlayerStartTag == "Player 2")
                {
                    /* Save this player start as player 2's for future reference. */
                    GameStatePtr->PlayerStarts.EmplaceAt(1, PlayerStart);

                    return PlayerStart;
                }
            }
        }
    }

    /* If the any conditions fail, just return the first player start. */
    return ClassArray[0];
}

void AMatch_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
    /*
     * Initialization sequence:
     * - (Gamemode) Both players log in and are assignd an index
     * - (Gamemode) Start match setup when both players connect
     * - (GameState) Match setup begins
     * - (GameState) [Some kind of map introduction.]
     * - (GameState) Players are given an initial status
     * - (GameState) Army selection widget is created
     * - (GameState) When both players have clicked the "ready" button, the match starts
     * - (GameState) The base widget is created and each player is given an initial status
     * - (GameState) The timer starts
     */

    Super::PostLogin(NewPlayer);

    /* This code only executes on the server. */
    if (HasAuthority())
    {
        /* If the new player has a match player state... */
        if (Cast<AMatch_PlayerState>(NewPlayer->PlayerState))
        {
            /* Set the new player's index to the total number of players in the game. */
            Cast<AMatch_PlayerState>(NewPlayer->PlayerState)->PlayerIndex = UGameplayStatics::GetGameState(this)->PlayerArray.Num();
        }

        /* If two players have connected... */
        if (GetNumPlayers() == 2)
        {
            /* Get the game state. */
            AMatch_GameStateBase* MatchGameState = GetWorld()->GetGameState<AMatch_GameStateBase>();
            
            if (MatchGameState)
            {
                /* Set up the match. */
                MatchGameState->SetUpMatch();

                /* Update the match's current status. */
                MatchGameState->SetMatchStatus(E_SettingUpPieces);
            }
        }
    }
}