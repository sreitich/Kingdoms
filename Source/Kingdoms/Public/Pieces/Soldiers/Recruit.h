// Copyright Samuel Reitich 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Recruit.generated.h"

class ABoardTile;

/**
 * 
 */
UCLASS()
class KINGDOMS_API ARecruit : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this character's properties. */
	ARecruit();

	/* Calls OnMoveToTileCompleted() so that Recruits placed onto the board adjacent to each other instantly receive
	 * their passive modifier. */
	virtual void OnGameStart() override;

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Returns all of this passive ability's valid targets. */
	virtual TArray<AActor*> GetValidPassiveAbilityTargets() override;

	/* Returns all tiles within range of this passive ability. Does not check for validity. */
	virtual TArray<ABoardTile*> GetPassiveAbilityRange() override;


/* Public variables. */
public:

	/* Used to identify when a piece is being placed for the first time, so that recruits don't trigger each other's
	 * pop-ups when the game starts. */
	UPROPERTY()
	bool bPlaced = false;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/* Triggers this piece's passive ability when it finishes moving to a new tile. */
	virtual void OnMoveToTileCompleted() override;

	/* Update the passive modifier depending on how many recruits are in the adjacent recruit array. */
	void UpdatePassiveModifier(bool bTriggerPopUp);


/* Protected variables. */
protected:

	UPROPERTY()
	TArray<FModifier> ModifiersFromPassive;

	UPROPERTY()
	TArray<ARecruit*> AdjacentRecruits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Passive Ability")
	int PassiveStrengthBuffPerPiece;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Passive Ability")
	int PassiveArmorBuffPerPiece;
};
