// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Match_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMatch_PlayerState : public APlayerState
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	AMatch_PlayerState();

	/* Replicates variables. */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Ends this player's turn and starts the next player's turn. This is a wrapper for the EndTurn RPC in the game state. */
	UFUNCTION(Server, Reliable)
	void Server_EndTurn();


/* Public accessors and modifiers. */
public:

	/* Getter for bReadyToPlay. */
	UFUNCTION(BlueprintPure, Category="Ready to Play")
	FORCEINLINE bool GetReadyToPlay() const { return bReadyToPlay; }

	/* Server-side setter for bReadyToPlay. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Ready to Play")
	void SetReadyToPlay_Server(bool bReady);

	/* Getter for CurrentPlayerStatus. */
	UFUNCTION(BlueprintPure, Category="Player Status")
	FORCEINLINE EPlayerStatus GetCurrentPlayerStatus() const { return CurrentPlayerStatus; }

	/* Client-side setter for CurrentPlayerStatus to remove client-side latency. */
	UFUNCTION(BlueprintCallable, Category="Player Status")
	void SetLocalPlayerStatus(EPlayerStatus NewPlayerStatus);
	
	/* Server-side setter for CurrentPlayerStatus. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Status")
	void Server_SetPlayerStatus(EPlayerStatus NewPlayerStatus);

	/* Getter for bMoveActionUsed. */
	UFUNCTION(BlueprintPure, Category="Turn Progress")
	FORCEINLINE bool GetMoveActionUsed() const { return bMoveActionUsed; }

	/* Server-side setter for bMoveActionUsed. Turn progress can only be reset by changing the player's state. This
	 * function can only be used to set bMoveActionUsed to true, assuming that the player is in a valid state. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Turn Progress")
	void Server_SetMoveActionUsed(bool bNewMoveActionUsed);

	/* Getter for bAbilityActionUsed. */
	UFUNCTION(BlueprintPure, Category="Turn Progress")
	FORCEINLINE bool GetAbilityActionUsed() const { return bAbilityActionUsed; }

	/* Server-side setter for bAbilityActionUsed. Turn progress can only be reset by changing the player's state. This
	 * function can only be used to set bAbilityActionUsed to true, assuming that the player is in a valid state. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Turn Progress")
	void Server_SetAbilityActionUsed(bool bNewAbilityActionUsed);


/* Public variables. */
public:

	/* Which of the two players this player state belongs to. */
	UPROPERTY(Replicated, BlueprintReadOnly)
	int PlayerIndex;

	/* An array of every piece that this player owns. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category="Player Information")
	TArray<AActor*> OwnedPieces;


/* Protected functions. */
protected:

	/* Calls additional logic in when. */
	UFUNCTION()
	void OnRep_CurrentPlayerStatus(EPlayerStatus OldPlayerStatus);

	/* Updates the player's turn progress UI and allows them to end their turn. */
	UFUNCTION()
	void OnRep_MoveActionUsed();

	/* Updates the player's turn progress UI and prompts them to end their turn. */
	UFUNCTION()
	void OnRep_AbilityActionUsed();


/* Protected variables. */
protected:

	/* Tracks when the player has placed their pieces and is ready to start the match. */
	UPROPERTY(Replicated, VisibleAnywhere)
	bool bReadyToPlay = false;

	/* Determines the current available actions for this player. */
	UPROPERTY(Replicated, VisibleAnywhere)
	TEnumAsByte<EPlayerStatus> CurrentPlayerStatus;

	/* Tracks whether the player has used their move/attack action for this turn. */
	UPROPERTY(ReplicatedUsing=OnRep_MoveActionUsed, VisibleAnywhere)
	bool bMoveActionUsed;

	/* Tracks whether the player has used their move/attack action for this turn. */
	UPROPERTY(ReplicatedUsing=OnRep_AbilityActionUsed, VisibleAnywhere)
	bool bAbilityActionUsed;

};
