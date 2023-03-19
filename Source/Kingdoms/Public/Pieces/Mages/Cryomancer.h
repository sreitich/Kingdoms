// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Pieces/ParentPiece.h"
#include "Cryomancer.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API ACryomancer : public AParentPiece
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

	/* Called when an effect given by this piece expires. */
	virtual void OnAbilityEffectEnded(TArray<AActor*> Targets) override;

		/* Removes this piece's ability indicator on all clients. */
		UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnAbilityEffectEnded();


/* Public variables. */
public:

	/* Used to maintain a valid reference to the ability target while the ability sequence plays, after the player's
	 * target has been reset. */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Active Ability")
	AParentPiece* ActiveAbilityTarget;

	/* The strength modification value of the Cryomancer's active ability modifier.. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int FrostboltStrengthChange = -1;

	/* The armor modification value of the Cryomancer's active ability modifier.. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int FrostboltArmorChange = -1;
	
	/* How long the Cryomancer's active ability lasts for. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Active Ability")
	int FrostboltFreezeDuration = 2;


/* Protected functions. */
protected:

	/* Executes blueprint implementation of active ability. */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnActiveAbility(AParentPiece* Target);

	/* Removes the ice particle effect that indicates that another piece was frozen by this piece. */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnAbilityEffectEnded();

};
