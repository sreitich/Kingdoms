// Copyright Change Studios, LLC 2023.

#pragma once

#include "Interfaces/MenuTransitionInterface.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_PlayMenuWidget.generated.h"

class UButton;
class UKingdomsGameInstance;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_PlayMenuWidget : public UUserWidget, public IMenuTransitionInterface
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


	/* Attempts to connect the player to an existing session. Creates a new session if one cannot be found. */
	UFUNCTION(BlueprintCallable)
	void TryQuickplay();

	/* Navigates to the lobby menu. */
	UFUNCTION(BlueprintCallable)
	void NavigateToLobbyMenu();

	/* Creates a host beacon and navigates to the lobby menu. Returns whether or not the host beacon was successfully
	 * created. */
	UFUNCTION(BlueprintCallable)
	bool CreateCustomGame();

	/* Plays the deactivation animation animation, which deactivates this widget when it finishes playing. */
	UFUNCTION(BlueprintCallable)
	void ReturnToMainMenu();


/* Protected variables. */
protected:

	/* A persistent pointer to the game instance. */
	UPROPERTY()
	UKingdomsGameInstance* GameInstancePtr;


/* Protected widgets. */
protected:

	/* Attempts to join an online match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* QuickPlayButton;

	/* Makes a new lobby to allow the user to create a custom game. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CustomGameButton;

	/* Returns the player to the main menu. */
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
