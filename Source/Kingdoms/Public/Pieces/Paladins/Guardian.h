// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Guardian.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AGuardian : public AParentPiece
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Replicates variables. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Tests if the given tile's coordinates match any of this piece's movement patterns.. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile) override;

	/* Creates and updates the active ability confirmation widget. */
	virtual void OnActiveClicked() override;

	/* Updates the active ability confirmation widget with the given targets. */
	virtual void Piece_UpdateActiveConfirmation(TArray<AActor*> Targets) override;

	/* Returns valid active ability targets within range. */
	virtual TArray<AActor*> GetValidActiveAbilityTargets() override;
	
	/* Returns tiles within range of this piece's active ability. */
	virtual TArray<ABoardTile*> GetActiveAbilityRange() override;

	/* Executes active ability logic. */
	virtual void OnActiveAbility(TArray<AActor*> Targets) override;

		/* Executes blueprint implementation of active ability. */
		UFUNCTION(BlueprintImplementableEvent)
		void BP_OnActiveAbility(AParentPiece* Target);


/* Protected variables. */
public:

	/* Used to maintain a valid reference to the ability target while the ability sequence plays, after the player's
	 * target has been reset. */
    UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Active Ability")
	AParentPiece* ActiveAbilityTarget;

	/* How much armor the Guardian's active ability provides. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int ArmorBuffValue = 2;

	/* How long the Guardian's armor buff lasts for. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int ArmorBuffDuration = 2;


};
