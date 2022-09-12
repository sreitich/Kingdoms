// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_ActiveAbilityConfirmation.generated.h"

class AParentPiece;

class UButton;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_ActiveAbilityConfirmation : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Getter for AbilityUser. */
	UFUNCTION(BlueprintPure, Category="Ability User")
	FORCEINLINE AParentPiece* GetAbilityUser() const { return AbilityUser; }

	/* Setter for AbilityUser. */
	UFUNCTION(BlueprintCallable, Category="Ability User")
	bool SetAbilityUser(AParentPiece* NewAbilityUser);

	/* Getter for PendingTargets. */
	UFUNCTION(BlueprintPure, Category="Pending Target")
	FORCEINLINE TArray<AActor*> GetPendingTargets() const { return PendingTargets; };

	/* Setter for PendingTargets. */
	UFUNCTION(BlueprintCallable, Category="Pending Target")
	bool SetPendingTargets(TArray<AActor*> NewTargets);

	/* Allows a target to be added to the array of targets, instead of setting the entire array. */
	UFUNCTION(BlueprintCallable, Category="Pending Target")
	bool AddPendingTarget(AActor* NewTarget);

	/* Resets the "use active ability" action. Public to be used by the player controller for proper
	 * cleanup when deselecting a piece while selecting a target. */
	UFUNCTION()
	void OnCancelClicked();


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Moves the selected piece to the selected tile. */
	UFUNCTION()
	void OnConfirmClicked();


/* Protected variables. */
protected:

	/* The piece whose active ability is being used. */
	UPROPERTY()
	AParentPiece* AbilityUser;

	/* The actor or actors that this ability is targeting. Different abilities target different types
	 * and different amounts of actors, so this needs to be an array of pointers to generic AActors. */
	UPROPERTY()
	TArray<AActor*> PendingTargets;


/* Protected widgets. */
protected:

	/* Button that confirms the player's move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ConfirmButton;

	/* Button that cancels the player's move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;


};
