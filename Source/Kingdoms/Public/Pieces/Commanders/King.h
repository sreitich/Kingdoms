// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/Commanders/ParentCommanderPiece.h"
#include "King.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AKing : public AParentCommanderPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this character's properties. */
	AKing();

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Returns all friendly pieces (ability has infinite range). */
	virtual TArray<ABoardTile*> GetActiveAbilityRange() override;

	/* Returns all friendly pieces (all friendly pieces are valid). */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;

	/* Creates an active ability confirmation widget since the player doesn't need to select a specific target. */
	virtual void OnActiveClicked() override;

	/* Wrapper for BP_OnActiveAbility. */
	virtual void OnActiveAbility(TArray<AActor*> Targets) override;

		/* Triggers the active ability animation, which triggers the active ability effect. */
		UFUNCTION(BlueprintImplementableEvent)
		void BP_OnActiveAbility();


/* Public constants. */
public:

	/* The effect that the King's active ability has on pieces' strength. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int AbilityStrengthMod = 0;

	/* The effect that the King's active ability has on pieces' armor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int AbilityArmorMod = 0;

	/* How long the King's active ability's effect lasts on pieces. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int AbilityModDuration = 0;

};
