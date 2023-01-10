// Copyright Change Studios, LLC 2023.

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

	/* Ends this player's turn and starts the next player's turn. This is a wrapper for the SwitchTurn RPC in the game state. */
	UFUNCTION(Server, Reliable)
	void Server_EndPlayerTurn();


/* Public accessors and modifiers. */
public:

	/* Getter for if the player is ready to start the match. */
	UFUNCTION(BlueprintPure, Category="Ready to Play")
	FORCEINLINE bool GetReadyToPlay() const { return bReadyToPlay; }

	/* Sets the player's "ready" state locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Ready to Play")
	void SetReadyToPlay(bool bReady);

	/* Getter for the player's current status. */
	UFUNCTION(BlueprintPure, Category="Player Status")
	FORCEINLINE EPlayerStatus GetCurrentPlayerStatus() const { return CurrentPlayerStatus; }
	
	/* Sets the player status locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Player Status")
	void SetPlayerStatus(EPlayerStatus NewPlayerStatus);

	/* Getter for whether the player has used their move action. */
	UFUNCTION(BlueprintPure, Category="Turn Progress")
	FORCEINLINE bool GetMoveActionUsed() const { return bMoveActionUsed; }

	/* Sets whether the player has used their move action locally and on the server. Turn progress can only be reset by
	 * changing the player's state. This function can only be used to set bMoveActionUsed to true, assuming that the
	 * player is in a valid state. */
	UFUNCTION(BlueprintCallable, Category="Turn Progress")
	void SetMoveActionUsed();

	/* Getter for whether the player has used their ability action. */
	UFUNCTION(BlueprintPure, Category="Turn Progress")
	FORCEINLINE bool GetAbilityActionUsed() const { return bAbilityActionUsed; }

	/* Sets whether the player has used their ability action locally and on the server. Turn progress can only be reset by
	 * changing the player's state. This function can only be used to set bAbilityActionUsed to true, assuming that the
	 * player is in a valid state. */
	UFUNCTION(BlueprintCallable, Category="Turn Progress")
	void SetAbilityActionUsed();

	/* Getter for whether this player is in a sequence. */
	UFUNCTION(BlueprintPure, Category="Sequences")
	FORCEINLINE bool GetIsInSequence() const { return bIsInSequence; }

	/* RPC wrapper for SetIsInSequence. */
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Sequences")
	void Client_SetIsInSequence(bool bNewIsInSequence);

		/* Sets whether this player is in a sequence and refreshes widgets depending on the new value. */
		UFUNCTION(BlueprintCallable, Category="Sequences")
		void SetIsInSequence(bool bNewIsInSequence);


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

	/* Attempts to start the match if both players are ready. */
	UFUNCTION()
	void OnRep_bReadyToPlay();
	
	/* Calls additional logic when the player's status is updated. */
	UFUNCTION()
	void OnRep_CurrentPlayerStatus(EPlayerStatus OldPlayerStatus);

	/* Updates the player's turn progress UI and allows them to end their turn. */
	UFUNCTION()
	void OnRep_MoveActionUsed();

	/* Updates the player's turn progress UI and prompts them to end their turn. */
	UFUNCTION()
	void OnRep_AbilityActionUsed();


/* Protected accessors and modifiers. */
protected:

	/* Sets the player's "ready" state on the server. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Ready to Play")
	void Server_SetReadyToPlay(bool bReady);

	/* Sets the player's status on the server. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Status")
	void Server_SetPlayerStatus(EPlayerStatus NewPlayerStatus);

	/* Sets that the player has used their move action on the server. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Turn Progress")
	void Server_SetMoveActionUsed();

	/* Sets that the player has used their ability action on the server. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Turn Progress")
	void Server_SetAbilityActionUsed();


/* Protected variables. */
protected:

	/* Tracks when the player has placed their pieces and is ready to start the match. */
	UPROPERTY(ReplicatedUsing=OnRep_bReadyToPlay, VisibleAnywhere)
	bool bReadyToPlay = false;

	/* Determines the current available actions for this player. */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentPlayerStatus, VisibleAnywhere)
	TEnumAsByte<EPlayerStatus> CurrentPlayerStatus;

	/* Tracks whether the player has used their move/attack action for this turn. */
	UPROPERTY(ReplicatedUsing=OnRep_MoveActionUsed, VisibleAnywhere)
	bool bMoveActionUsed;

	/* Tracks whether the player has used their move/attack action for this turn. */
	UPROPERTY(ReplicatedUsing=OnRep_AbilityActionUsed, VisibleAnywhere)
	bool bAbilityActionUsed;

	/* Tracks whether a player is currently in a sequence (move, attack, or ability), preventing them from taking
	 * actions. */
	UPROPERTY(VisibleAnywhere)
	bool bIsInSequence;

};