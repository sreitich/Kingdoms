// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/SaveGameData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchSetup_PlacePieces.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatchSetup_PlacePieces : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Adds a card for each piece in a given army. */
	UFUNCTION()
	void LoadArmy(FArmyPresetStruct ArmyToLoad);

	/* Plays the HideCards animation to hide all remaining cards off-screen. */
	UFUNCTION()
	void PlayHideCardsAnim(bool bReverse);

	/* Enables the ReadyButton if all of the player's pieces have been placed. */
	UFUNCTION()
	void CheckAllPiecesPlaced() const;


/* Public variables. */
public:

	/* Hides all remaining piece cards off-screen. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HideCardsAnim;


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Bound to the ReadyButton widget. Attempts to start the match if both players have clicked it. */
	UFUNCTION()
	void OnReadyClicked();


/* Protected widget class types. */
protected:
    
	/* Widget blueprint to create when listing unplaced pieces. */
	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> MatchSetup_UnplacedPieceWidgetClass;


/* Protected widgets. */
protected:

	/* Box containing the unplaced pieces in this army. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UScrollBox* PiecesBox;

	/* Button that attempts to start the match if both players have clicked it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* ReadyButton;

};
