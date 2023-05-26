// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_ArmyPresetPreview.generated.h"

class UButton;
class UDataTable;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_ArmyPresetPreview : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	virtual void NativeConstruct() override;

	void UpdateArmyPresetPreview(uint8 NewPresetSlot);

/* Public variables. */
public:

	/* The index of which slot this preset is saved in. */
	UPROPERTY(BlueprintReadOnly)
	uint8 PresetSlot = 0;

/* Protected variables. */
protected:

	/* A reference to the piece data table. */
	UPROPERTY(EditDefaultsOnly)
	UDataTable* PieceDataTable;

/* Protected function. */
protected:

	UFUNCTION()
	void OnPresetButtonClicked();

/* Protected widgets. */
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PresetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* SelectPresetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* Piece1Preview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* Piece2Preview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* Piece3Preview;
};
