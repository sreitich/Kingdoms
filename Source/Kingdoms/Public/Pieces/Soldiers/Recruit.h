// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Recruit.generated.h"

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
	
};
