// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Guardian.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AGuardian : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;
};
