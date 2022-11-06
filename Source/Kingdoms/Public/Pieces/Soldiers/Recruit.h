// Copyright Samuel Reitich 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Recruit.generated.h"

class ABoardTile;

/**
 * 
 */
UCLASS()
class KINGDOMS_API ARecruit : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this character's properties. */
	ARecruit();

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/* Triggers this piece's passive ability when it finishes moving to a new tile. */
	virtual void OnMoveToTileCompleted() override;

	/* Returns "true" if the given tile is laterally adjacent to this piece's tile. */
	bool TileIsAdjacent(ABoardTile* Tile);


/* Protected variables. */
protected:

	UPROPERTY()
	TArray<FModifier> ModifiersFromPassive;
};
