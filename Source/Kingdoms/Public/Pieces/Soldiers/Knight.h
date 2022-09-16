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
	
	/* Sets default values for this character's properties. */
	AKnight();

	/* Implementation of "Dash" ability. */
	virtual void OnActiveAbility(AActor* Target) override;

	/* Blueprint implementation of "Dash" ability, called by OnActiveAbility(). */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnActiveAbility(AActor* Target);

	/* Valid targets for "Dash" ability. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

};
