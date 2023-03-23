// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Captain.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API ACaptain : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Returns all of this passive ability's valid targets. */
	virtual TArray<AActor*> GetValidPassiveAbilityTargets() override;
	
	/* Returns all tiles within range of this passive ability. Does not check for validity. */
	virtual TArray<ABoardTile*> GetPassiveAbilityRange() override;

	/* Calls OnTurnStart if this is piece is owned by the player taking their turn first. */
	virtual void OnGameStart() override;
	
	/* Activates this piece's passive ability animation on each client, activating its passive ability, if there are any
	 * pieces within range. */
	virtual void OnTurnStart() override;

		/* Plays this piece's passive ability animation on each client. */
		UFUNCTION(NetMulticast, Reliable)
		void Multicast_AnimatePassiveAbility();

	/* Activates this piece's passive ability. Blueprint-exposed wrapper for OnPassiveAbility.*/
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	void TriggerOnPassiveAbility();

		/* Implementation of this piece's passive ability. */
		virtual void OnPassiveAbility(TArray<AActor*> Targets) override;


/* Protected variables. */
protected:

	/* How much this piece's passive ability adjusts the target's strength by. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Passive Ability")
	int PassiveAbilityStrengthChange;

	/* How much this piece's passive ability adjusts the target's armor by. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Passive Ability")
	int PassiveAbilityArmorChange;
};
