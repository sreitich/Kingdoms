// Copyright Change Studios, LLC 2023.

#pragma once

#include "Interfaces/MenuTransitionInterface.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_ArmyMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_ArmyMenuWidget : public UUserWidget, public IMenuTransitionInterface
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Plays the deactivation animation, which destroys this widget and transitions to the next menu in the HUD when it
	 * finishes playing. */
	virtual void DeactivateWidget() override;


/* Protected functions. */
protected:

	/* Called when this widget is created. */
	virtual void NativeConstruct() override;

	/* Called when the deactivation animation finishes playing to destroy this widget. */
	UFUNCTION()
	void OnDeactivatedAnimEnd();


	/* Transitions to the collection menu widget. */
	UFUNCTION()
	void OnCollectionClicked();

	/* Returns to the main menu. */
	UFUNCTION()
	void OnBackClicked();


/* Protected widgets. */
protected:

	/* Navigates to the collection menu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CollectionButton;

	/* Returns the user to the main menu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* BackButton;


/* Protected animations. */
protected:

	/* Played when this widget is spawned. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnActivatedAnim;

	/* Played when this widget is destroyed. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnDeactivatedAnim;

};
