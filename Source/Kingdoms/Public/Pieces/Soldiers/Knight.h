// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Knight.generated.h"

class ABoardTile;

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

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Valid targets for "Dash" ability. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

	/* Creates a confirmation widget. */
	virtual void StartActiveConfirmation(TArray<AActor*> Targets) override;

	/* Implementation of "Dash" ability. */
	virtual void OnActiveAbility(TArray<AActor*> Targets) override;

		/* Blueprint implementation of "Dash" ability, called by OnActiveAbility(). */
		UFUNCTION(BlueprintImplementableEvent)
		void BP_OnActiveAbility(ABoardTile* Target);


/* Public variables. */
public:

	UPROPERTY()
	class UKnight_ActiveAbilityConfirmation* ConfirmationWidget;

};
