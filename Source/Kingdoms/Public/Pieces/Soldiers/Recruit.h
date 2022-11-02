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
	
};
