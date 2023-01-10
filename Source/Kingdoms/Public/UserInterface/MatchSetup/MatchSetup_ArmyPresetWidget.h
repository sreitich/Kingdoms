// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/SaveGameData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchSetup_ArmyPresetWidget.generated.h"

class UButton;
class UMatchSetup_SelectArmy;
class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatchSetup_ArmyPresetWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates this widget's displayed information. */
	void UpdateArmyPresetWidget();


/* Public variables. */
public:

	/* A reference to this widget's owning widget; used to update the parent widget's variables. */
	UPROPERTY()
	UMatchSetup_SelectArmy* ParentWidget;

	/* Which player-created army preset this widget represents. */
	FArmyPresetStruct ArmyPreset;


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Bound to the select button widget. Selects this army preset, highlights it, and deselects other the other presets. */
	UFUNCTION()
	void OnSelectClicked();


/* Protected widgets. */
protected:
	
	/* Selects this army preset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* SelectButton;

	/* Displayed army name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ArmyName;
	
};
