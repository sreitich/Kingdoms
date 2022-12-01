// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/Commanders/ParentCommanderPiece.h"
#include "King.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AKing : public AParentCommanderPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this character's properties. */
	AKing();

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;
	
};
