// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_CollectionInfoPopUpWidget.generated.h"

class UButton;
class UHorizontalBox;
class UImage;
class URichTextBlock;
class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_CollectionInfoPopUpWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. Binds this function's buttons. */
	virtual void NativeConstruct() override;

	/* Updates the displayed piece information and reveal the widget with a pop-up animation. */
	UFUNCTION()
	void UpdateAndActivatePopUp(FPieceDataStruct PieceData);

	/* Safely destroys this widget through the HUD. */
	UFUNCTION()
	void ClosePopUp();


/* Protected widgets. */
protected:

	// A preview image of the selected piece
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PreviewImage;

	// The name of the piece
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PieceName;

	// The piece's rarity and class in the format "Rarity Class"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* PieceTypeText;

	// The piece's description
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* PieceDescription;

	// The piece's displayed base strength value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* StrengthText;

	// The piece's displayed base armor value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* ArmorText;

	// This piece's passive ability icon, if it has one
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PassiveAbilityIcon;

	// This piece's passive ability text, in the format "Name: Description"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* PassiveAbilityText;

	// This piece's active ability icon, if it has one
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* ActiveAbilityIcon;

	// This piece's active ability text, in the format "Name: Description"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* ActiveAbilityText;

	// Destroys this widget when clicked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CloseButton;

	// Destroys this widget when clicking anywhere outside of the pop-up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* BackgroundButton;

	// Static slot for the first listed ability.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UHorizontalBox* TopAbilityWrapper;

	// Parent widget for the passive ability info.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UHorizontalBox* PassiveBox;

	// Static slot for the second listed ability.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UHorizontalBox* BottomAbilityWrapper;

	// Parent widget for the active ability info.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UHorizontalBox* ActiveBox;


/* Protected animations. */
protected:

	
};
