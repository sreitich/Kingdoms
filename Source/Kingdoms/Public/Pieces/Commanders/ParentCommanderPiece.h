// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "ParentCommanderPiece.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AParentCommanderPiece : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Ends the game when this piece dies. */
	virtual void OnDeath(AActor* Killer) override;
	
};
