// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Knight.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AKnight : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Gives this piece a unique piece ID that can be accessed from anywhere. */
	FName GetPieceID() override;
	
	/* Sets default values for this character's properties. */
	AKnight();

};
