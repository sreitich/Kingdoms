// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_AbilityInfoPopup.generated.h"

class AParentPiece;

class UTextBlock;
class UOverlay;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_AbilityInfoPopup : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called when this widget is created. Updates displayed name and description from the piece data
	 * table by default. */
	UFUNCTION()
	virtual void SetUpWidget(AParentPiece* DisplayedPiece, bool bActive);


/* Public widgets. */
public:

	/* The overlay encompassing the entire widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* AbilityInfoOverlay;


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;


/* Protected widgets. */
protected: 
	
	/* Displayed name of the ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedAbilityName;

	/* Displayed description of the ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedAbilityDescription;
	
};
