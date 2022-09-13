// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Pyromancer.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API APyromancer : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:
	
	/* Sets default values for this character's properties. */
	APyromancer();

	/* Implementation of "Dash" ability. */
	virtual void OnActiveAbility(AActor* Target) override;

	/* Valid targets for "Dash" ability. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;
};
