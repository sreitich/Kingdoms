// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Assassin.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AAssassin : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Returns all valid tiles that this piece can currently move to. Checks for validity. */
	virtual TArray<ABoardTile*> GetValidMoveTiles() override;

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Creates and updates the Assassin's active ability confirmation widget. */
	virtual void OnActiveClicked() override;

	/* Updates the Assassin's active ability confirmation widget. */
	virtual void Piece_UpdateActiveConfirmation(TArray<AActor*> Targets) override;

	/* Valid targets for the "Backstab" ability. Returns all valid tile locations, but doesn't check if
	 * there are enemy pieces occupying them. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

		/* Returns all of the tiles within the backstab's range, regardless of if they have valid target pieces. */
		virtual TArray<ABoardTile*> GetActiveAbilityRange() override;

	/* Implementation of "Backstab" ability. */
	virtual void OnActiveAbility(TArray<AActor*> Targets) override;

		/* Blueprint implementation of "Backstab" ability, called by OnActiveAbility(). */
		UFUNCTION(BlueprintImplementableEvent)
		void BP_OnActiveAbility(AParentPiece* Target);


public:

	/* How much strength the Assassin gains when using Backstab. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int StrengthBuffValue = 2;
};
