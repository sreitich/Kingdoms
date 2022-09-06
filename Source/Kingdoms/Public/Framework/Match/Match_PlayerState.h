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


/* Public variables. */
public:

	/* Which of the two players this player state belongs to. */
	UPROPERTY(Replicated)
	int PlayerIndex;

	/* An array of every piece that this player owns. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category="Player Information")
	TArray<AActor*> OwnedPieces;


/* Protected functions. */
protected:

	/* Removes place pieces widgets and starts match if all players are now ready. */
	UFUNCTION()
	void OnRep_ReadyToPlay();

	/* Player's available actions change with the player status. */
	UFUNCTION()
	void OnRep_CurrentPlayerStatus();


/* Protected variables. */
protected:

	/* Tracks when the player has placed their pieces and is ready to start the match. */
	UPROPERTY(ReplicatedUsing=OnRep_ReadyToPlay, VisibleAnywhere)
	bool bReadyToPlay = false;

	/* Determines the current available actions for this player. */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentPlayerStatus, VisibleAnywhere)
	TEnumAsByte<EPlayerStatus> CurrentPlayerStatus;

};
