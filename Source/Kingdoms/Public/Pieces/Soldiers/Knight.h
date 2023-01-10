// Copyright Change Studios, LLC 2023.

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

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Creates and updates the knight's active ability confirmation widget. */
	virtual void OnActiveClicked() override;

	/* Updates the knight's active ability confirmation widget. */
	virtual void Piece_UpdateActiveConfirmation(TArray<AActor*> Targets) override;

	/* Valid targets for "Dash" ability. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

		/* Returns all tiles within the active ability's range. This is currently unused because "Dash" uses the
		 * Knight's movement range. */
		virtual TArray<ABoardTile*> GetActiveAbilityRange() override;

	/* Implementation of "Dash" ability. */
	virtual void OnActiveAbility(TArray<AActor*> Targets) override;

		/* Blueprint implementation of "Dash" ability, called by OnActiveAbility(). */
		UFUNCTION(BlueprintImplementableEvent)
		void BP_OnActiveAbility(ABoardTile* Target);

};
