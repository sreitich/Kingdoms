// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_AbilityInfo.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_AbilityInfo : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called every frame. */
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	/* Updates the name and description of this widget. */
	UFUNCTION()
	void UpdateAbilityInfo(FString NewName, FString NewDescription);


/* Protected widgets. */
protected: 

	/* Displayed name of the hovered ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedAbilityName;

	/* Displayed desciption of the hovered ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedAbilityDescription;

};
