// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PieceAIController.generated.h"

/* Forward declaration of BoardTile class. */
class ABoardTile;

UCLASS()
class KINGDOMS_API APieceAIController : public AAIController
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	APieceAIController();

	/* Moves the piece to a new tile. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void MovePieceToTile(ABoardTile* NewTile, bool bResetStateWhenFinished);

	/* Called when a move request is completed. */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;


/* Public variables. */
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	bool bResetAfterMove = false;
};
