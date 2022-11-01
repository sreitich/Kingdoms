// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "AcademyRecruit.generated.h"

class ABoardTile;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AAcademyRecruit : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:
	
	/* Sets default values for this character's properties. */
	AAcademyRecruit();

	/* Returns all tiles that this piece can move to. */
	virtual TArray<ABoardTile*> GetValidMoveTiles() override;

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;
	
};
