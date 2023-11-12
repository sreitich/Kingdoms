// Copyright Change Studios, LLC 2023.

#pragma once

#include "Interfaces/MenuTransitionInterface.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_CollectionMenuWidget.generated.h"

class UButton;
class UUniformGridPanel;

class UDataTable;

class UPieceCollectionListing;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_CollectionMenuWidget : public UUserWidget, public IMenuTransitionInterface
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

	/* Called when this widget is destroyed to clear its children. */
	virtual void NativeDestruct() override;

	/* Called when the deactivation animation finishes playing to destroy this widget. */
	UFUNCTION()
	void OnDeactivatedAnimEnd();


	/* Plays the deactivation animation animation, which deactivates this widget when it finishes playing. */
	UFUNCTION()
	void OnBackClicked();


/* Protected variables. */
protected:

	/* The game's standard piece data table. */
	UPROPERTY(EditDefaultsOnly, Category="Piece Listings")
	UDataTable* PieceDataTable;

	/* The piece listing widget to spawn. */
	UPROPERTY(EditDefaultsOnly, Category="Piece Listings")
	TSubclassOf<UPieceCollectionListing> PieceListingClass;


/* Protected widgets. */
protected:

	/* Contains the list of pieces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UUniformGridPanel* PieceListingGrid;

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