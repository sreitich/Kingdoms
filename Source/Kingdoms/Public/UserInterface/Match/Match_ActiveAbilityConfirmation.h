// Copyright Change Studios, LLC 2023.

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

	/* Updates this widget. Default implementation updates the ability-using piece and the targets if the given ones are
	 * valid. If invalid targets were given, disables the "confirm" button. Otherwise, enables it. */
	UFUNCTION()
	virtual void Widget_UpdateActiveConfirmation(AParentPiece* NewPieceUsingAbilityNewPieceUsingAbility, TArray<AActor*> Targets);

	/* Getter for AbilityUser. */
	UFUNCTION(BlueprintPure, Category="Ability User")
	FORCEINLINE AParentPiece* GetPieceUsingAbility() const { return PieceUsingAbility; }

	/* Setter for PieceUsingAbility. */
	UFUNCTION(BlueprintCallable, Category="Ability User")
	bool SetPieceUsingAbility(AParentPiece* NewPieceUsingAbility);

	/* Getter for PendingTarget. */
	UFUNCTION(BlueprintPure, Category="Pending Targets")
	FORCEINLINE TArray<AActor*> GetPendingTargets() const { return PendingTargets; };

	/* Setter for PendingTarget. */
	UFUNCTION(BlueprintCallable, Category="Pending Targets")
	bool SetPendingTargets(TArray<AActor*> NewTargets);

	/* Cleans up and destroys this widget. Resets the player's state, all selected actors, and all tiles. */
	UFUNCTION()
	virtual void DestroyWidget();


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Moves the selected piece to the selected tile. */
	UFUNCTION()
	virtual void OnConfirmClicked();

	/* Destroys this widget and resets the player and all tiles. */
	UFUNCTION()
	virtual void OnCancelClicked();


/* Protected variables. */
protected:

	/* The piece whose active ability is being used. */
	UPROPERTY()
	AParentPiece* PieceUsingAbility;

	/* The actors this ability is targeting. Different abilities target different types of actors, so this needs to be a pointer to
	 * generic AActors. This could be changed to a TArray to support multiple targets, but no abilities currently have more than one target. */
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
