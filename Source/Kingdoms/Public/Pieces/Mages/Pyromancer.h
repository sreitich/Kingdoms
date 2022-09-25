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

	/* Lightly highlights all potential targets and fully highlights valid targets. */
	virtual void OnActiveClicked() override;
	
	/* Valid targets for "Fireball" ability. Returns all valid tile locations, but doesn't check if
	 * there are enemy pieces occupying them. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

		/* Returns all of the tiles within the fireball's range, regardless of if they have valid target pieces. */
		virtual TArray<AActor*> GetActiveAbilityRange() override;

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


/* Public assets. */
public:

	/* Played when this piece uses its active ability when powering up before using
	 * its active ability animation to attack. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* PowerUpAnimation;
};
