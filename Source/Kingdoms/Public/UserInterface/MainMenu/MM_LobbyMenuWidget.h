// Copyright Change Studios, LLC 2023.

#pragma once

#include "Interfaces/MenuTransitionInterface.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_LobbyMenuWidget.generated.h"

class UButton;
class UMM_PlayMenuWidget;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_LobbyMenuWidget : public UUserWidget, public IMenuTransitionInterface
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

	/* Transitions to the map selection menu. Called when the player clicks to change the selected map. */
	UFUNCTION(BlueprintCallable)
	void OpenMapSelection();
	
	/* Transitions back to the play menu. Called when the player clicks "back." */
	UFUNCTION(BlueprintCallable)
	void ReturnToPlayMenu();


/* Protected animations. */
protected:

	/* Played when this widget is spawned. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnActivatedAnim;

	/* Played when this widget is destroyed. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnDeactivatedAnim;
	
};
