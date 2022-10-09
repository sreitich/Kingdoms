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
		void BP_OnActiveAbility(AParentPiece* Target);

	/* Stops the Pyromaner's flaming hands. */
	virtual void OnActiveEffectEnded(TArray<AActor*> Targets) override;


/* Public variables. */
public:

	/* Pointer to the currently displayed active ability confirmation widget. */
	UPROPERTY()
	class UMatch_PyroActiveConfirmation* ConfirmationWidget;

	/* Tracks whether or not the piece's power-up animation has played yet. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Active Ability")
	bool bPowered = false;

	/* How much strength the Pyromancer gains from casting Fireball. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int StrengthBuff = 2;


/* Public assets. */
public:

	/* Played when this piece attacks using a "move" action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* MoveAttackAnimation;

	/* Played when this piece attacks using its active ability, "Fireball." */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* FireballAttackAnimation;
};
