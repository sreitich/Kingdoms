// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/SaveGameData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchSetup_SelectArmy.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatchSetup_SelectArmy : public UUserWidget
{
	GENERATED_BODY()

/* Public variables. */
public:

	/* Which army preset the player currently has selected. */
	FArmyPresetStruct SelectedArmy;


/* Public widget class types. */
public:

	/* Widget blueprint to create when listing army presets. */
	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> MatchSetup_ArmyPresetWidgetClass;


/* Public widgets. */
public:

	/* Button that selects the army and allows the player to start placing pieces. Needs to be accessible by army preset widgets. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
    class UButton* SelectArmyButton;
	

/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;
	
	/* Bound to the SelectArmyButton widget. Selects the army and allows the player to start placing pieces. */
	UFUNCTION()
	void OnSelectArmyClicked();


/* Protected widgets. */
protected:

	/* Horizontal box containing all of the army preset widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UHorizontalBox* ArmyListBox;

};
