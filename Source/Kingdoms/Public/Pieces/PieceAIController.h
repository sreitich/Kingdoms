// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PieceAIController.generated.h"

class ABoardTile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveCompleted);

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

	/* Broadcasts the MoveCompleted delegate when this piece moves to a new tile. */
	UFUNCTION(BlueprintCallable)
	void OnMoveToTileCompleted();

	/* Called when a move request is completed. */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;


/* Public variables. */
public:

	/* Delegated broadcast when this piece finishes moving to its destination. */
	UPROPERTY(BlueprintAssignable)
	FMoveCompleted MoveCompleted;

	/* Whether or not this piece's rotation should be reset after moving. If this piece is moving to attack another
	 * piece, it shouldn't turn away from the piece when it reaches it. But if a piece is moving tiles or returning to a
	 * tile, it should return to its original rotation afterwards. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	bool bResetAfterMove = false;


/* Protected functions. */
protected:

	/* Called when this actor possesses a pawn. */
	virtual void OnPossess(APawn* InPawn) override;
};
