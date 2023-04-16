// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_CollectionInfoPopUpWidget.generated.h"

class UButton;
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

	/* Updates the displayed piece information and reveal the widget with a pop-up animation. */
	UFUNCTION()
	void UpdateAndActivatePopUp(FPieceDataStruct PieceData);


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


/* Protected animations. */
protected:

	
};
