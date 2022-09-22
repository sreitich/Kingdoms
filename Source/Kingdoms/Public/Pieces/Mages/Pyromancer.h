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

	/* Valid targets for "Fireball" ability. All targets are enemy pieces. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

	/* Creates a confirmation widget. */
	virtual void StartActiveConfirmation(TArray<AActor*> Targets) override;

	/* Implementation of "Fireball" ability. */
	virtual void OnActiveAbility(TArray<AActor*> Targets) override;

		/* Blueprint implementation of "Pyromancer" ability, called by OnActiveAbility(). */
		UFUNCTION(BlueprintImplementableEvent)
		void BP_OnActiveAbility(ABoardTile* Target);


/* Public variables. */
public:

	UPROPERTY()
	class UMatch_AttackConfirmation* ConfirmationWidget;
};
