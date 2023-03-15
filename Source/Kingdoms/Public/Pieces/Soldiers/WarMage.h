// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "WarMage.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AWarMage : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;


/* Protected functions. */
protected:

	/* Adds an additional modifier when a new modifier is added. This is the implementation of this piece's passive
	 * ability. */
	virtual void OnModifierAddedOrRemoved(FModifier Modifier, bool bRemoved) override;
	
};
