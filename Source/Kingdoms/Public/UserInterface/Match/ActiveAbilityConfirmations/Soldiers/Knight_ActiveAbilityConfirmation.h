// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Knight_ActiveAbilityConfirmation.generated.h"

class ABoardTile;
class AKnight;

class UButton;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UKnight_ActiveAbilityConfirmation : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Gets references to the user of this ability and the target of this ability. */
	UFUNCTION()
	void UpdateActionConfirmationInfo(AKnight* NewAbilityUser, ABoardTile* NewTargetTile);


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Activates the ability using the current user and target. */
	UFUNCTION()
	void OnConfirmClicked();

	/* Called when destroyed. */
	virtual void NativeDestruct() override;

	/* Wrapper for NativeDestruct, which resets the game's state before destroying this widget. */
	UFUNCTION()
	void OnCancelClicked();


/* Protected variables. */
protected:

	/* A reference to the piece that will execute this ability. */
	UPROPERTY()
	AKnight* AbilityUser;

	/* A reference to the target of this ability. */
	UPROPERTY()
	ABoardTile* TargetTile;


/* Protected widgets. */
protected:

	/* Button that activates this ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ConfirmButton;

	/* Button that cancels this ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;

};
