// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PieceDragWidget.generated.h"

class AParentPiece;
class UUserWidget;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UPieceDragWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Initializes the widget depending on whether or not it needs to spawn its piece, or if the piece has already been spawned. */
	UFUNCTION()
	void InitializeWidget(bool bSpawnPiece);


/* Public variables. */
public:

	/* Class that this widget is responsible for spawning. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Classes")
	TSubclassOf<AParentPiece> ClassToSpawn;
	
	/* The originally dragged card that this widget represents. */
	UPROPERTY()
	UUserWidget* RepresentedWidget;

	/* The piece that this widget spawns and sets the location of. */
	UPROPERTY()
	AParentPiece* SpawnedPiece;

	/* Tracks when the player is no longer dragging this widget. Used to ensure the widget's code finishes executing
	 * before being destroyed. */
	UPROPERTY()
	bool bPendingDestroy;


/* Public widgets. */
public:

	/* A dummy unplaced piece widget that visually mimics the original widget that this new widget represents. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UMatchSetup_UnplacedPieceWidget* UnplacedPieceWidget;

	
/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Called every frame. */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* Called when the player stops dragging the piece. */
    virtual void NativeDestruct() override;

	/* Called when the transition animation finishes. Hides this widget. */
	UFUNCTION()
	void TransitionAnimFinished();

	/* Helper function that either performs a line trace for board tiles to the position of the player's mouse or finds the intersection between that line and the board tile plane. */
	UFUNCTION()
	FHitResult TraceFromMouse() const;


/* Private functions. */
private:

	/* Returns the closest unoccupied tile to the given location. */
	UFUNCTION()
	AActor* GetClosestOpenTile(const FVector& CloseToLocation) const;


/* Protected variables. */
protected:
	
	/* Enables binding a function to when an animation finishes. */
	FWidgetAnimationDynamicEvent FinishedDelegate;

	/* The height of board tiles on this level. */
	UPROPERTY()
	float BoardTileHeight;

	/* The current board tile that the dragged piece was placed onto. Used when updating the piece's position. */
	UPROPERTY()
	AActor* CurrentTile;

};
