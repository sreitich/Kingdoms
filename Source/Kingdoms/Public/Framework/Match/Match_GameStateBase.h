// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Match_GameStateBase.generated.h"

class ABoardManager;

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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Lets players start placing their pieces. */
	UFUNCTION()
	void SetUpMatch();

	/* Checks if both players have placed all of their pieces and are ready to start the match. */
	UFUNCTION()
	bool CheckReadyToStart();

	/* Initiates the game for all clients and begins the match on the server once both players have set up their pieces. */
	UFUNCTION(Server, Reliable)
	void Server_StartMatch();

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
	int32 MatchTime;

	/* Tracks the official time of the current player's turn. */
	UPROPERTY(Replicated)
	int32 TurnTime;

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

	/* Decrements the remaining durations of each piece's modifiers if they don't have an indefinite duration. */
	UFUNCTION(Server, Reliable, BlueprintCallable) // Remove the BlueprintCallable specifier after turns are implemented
	void Server_DecrementModifierDurations();


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