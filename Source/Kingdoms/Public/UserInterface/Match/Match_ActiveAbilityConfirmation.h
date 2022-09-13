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

	/* Updates the widget's displayed and internal information. Only used when creating this widget. */
	UFUNCTION()
	void UpdateActionConfirmationInfo(AParentPiece* NewAbilityUser, AActor* Target);

	/* Getter for AbilityUser. */
	UFUNCTION(BlueprintPure, Category="Ability User")
	FORCEINLINE AParentPiece* GetAbilityUser() const { return AbilityUser; }

	/* Setter for AbilityUser. */
	UFUNCTION(BlueprintCallable, Category="Ability User")
	bool SetAbilityUser(AParentPiece* NewAbilityUser);

	/* Getter for PendingTarget. */
	UFUNCTION(BlueprintPure, Category="Pending Target")
	FORCEINLINE AActor* GetPendingTarget() const { return PendingTarget; };

	/* Setter for PendingTarget. */
	UFUNCTION(BlueprintCallable, Category="Pending Target")
	bool SetPendingTarget(AActor* NewTarget);

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

	/* The actor this ability is targeting. Different abilities target different types of actors, so this needs to be a pointer to
	 * generic AActors. This could be changed to a TArray to support multiple targets, but no abilities currently have more than one target. */
	UPROPERTY()
	AActor* PendingTarget;


/* Protected widgets. */
protected:

	/* Button that confirms the player's move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ConfirmButton;

	/* Button that cancels the player's move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;


};
