// Copyright Change Studios, LLC 2023.

#pragma once

#include "Interfaces/MenuTransitionInterface.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_MainMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_MainMenuWidget : public UUserWidget, public IMenuTransitionInterface
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


	/* Called when the "play" button is clicked. Navigates to the "play" menu. */
	UFUNCTION()
	void OnPlayButton();

	/* Called when the "army" button is clicked. Navigates to the army menu. */
	UFUNCTION()
	void OnArmyButton();

	/* Called when the "store" button is clicked. Navigates to the in-game store. */
	UFUNCTION()
	void OnStoreButton();

	/* Called when the "options" button is clicked. Navigates to the options menu. */
	UFUNCTION()
	void OnOptionsButton();


/* Protected widgets. */
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* PlayNavigationButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ArmyNavigationButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* StoreNavigationButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* OptionsNavigationButton;


/* Protected animations. */
protected:

	/* Played when this widget is spawned. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnActivatedAnim;

	/* Played when this widget is destroyed. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnDeactivatedAnim;

};
