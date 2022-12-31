// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Match_GameStateBase.generated.h"

class ABoardManager;
class AMatch_PlayerState;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMatch_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	AMatch_GameStateBase();

	/* Replicates variables. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Lets players start placing their pieces. */
	UFUNCTION()
	void SetUpMatch();

	/* Checks if both players have placed all of their pieces and are ready to start the match. */
	UFUNCTION()
	bool CheckReadyToStart();

	/* Initiates the game for all clients and begins the match on the server once both players have set up their pieces. */
	UFUNCTION(Server, Reliable)
	void Server_StartMatch();

	/* Switches the current turn from the given player to the other player. */
	UFUNCTION(Server, Reliable)
	void Server_SwitchTurn(AMatch_PlayerState* CurrentPlayer);

	/* Ends the match with one winner and one loser. */
	UFUNCTION(Server, Reliable)
	void Server_EndMatch(AMatch_PlayerState* Winner);

	/* Getter for CurrentMatchStatus. */
	UFUNCTION(BlueprintPure, Category="Match Status")
	FORCEINLINE EMatchStatus GetCurrentMatchStatus() const { return CurrentMatchStatus; }

	/* Server-side setter for CurrentMatchStatus. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Match Status")
	void SetMatchStatus(EMatchStatus NewMatchStatus);


/* Public variables. */
public:

	/* Public references to each player's start. Player 1's start is the first element, player 2's is the second, etc. */
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<AActor*> PlayerStarts;
	
	/* A global pointer to this level's tile manager. */
	UPROPERTY(Replicated, BlueprintReadOnly)
	ABoardManager* BoardManager;
	
	/* Tracks the official time of the match. */
	UPROPERTY(Replicated)
	int32 MatchTime = 0.0f;

	/* Tracks the official time of the current player's turn. */
	UPROPERTY(Replicated)
	int32 TurnTime = 0.0f;

	/* Name of map, displayed in each player base widget. */
	FName MapName = "Clockwork";

	/* Used to ensure that only one player asks the server to animate pieces. */
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bAnimatedPiece;

	/* Maintains the information of the current attack. */
	UPROPERTY(Replicated, BlueprintReadWrite)
	FAttackInfo CurrentAttackInfo;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/* Match progresses with the match status. */
	UFUNCTION()
	void OnRep_CurrentMatchStatus();

	/* Reveals every piece to both players after the match starts. */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RevealAllPieces();

	/* Decrements the remaining durations of each piece's modifiers if they don't have an indefinite duration. */
	UFUNCTION(Server, Reliable)
	void Server_DecrementModifierDurations(AMatch_PlayerState* OwningPlayer);

	/* Decrements any active cooldowns for pieces owned by the given player. */
	UFUNCTION(Server, Reliable)
	void Server_DecrementAbilityCooldowns(AMatch_PlayerState* OwningPlayer);


/* Protected variables */
protected:

	/* Determines the state of the current match. */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentMatchStatus)
	TEnumAsByte<EMatchStatus> CurrentMatchStatus;


/* Private functions. */
private:

	// Increments the match time every second.
	UFUNCTION(Server, Reliable)
	void Count();


/* Private variables. */
private:

	/* Prevents the match from starting multiple times. */
	bool bMatchStarted;

	/* Handles the match timer. */
	FTimerHandle MatchTimeTimerHandle;

};