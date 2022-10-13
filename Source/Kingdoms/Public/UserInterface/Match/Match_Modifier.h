// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_Modifier.generated.h"

class URichTextBlock;

class UDataTable;

struct FModifier;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_Modifier : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	UFUNCTION(BlueprintCallable)
	void UpdateDisplayedModifier(FModifier NewModifier, bool bAlignedLeft);


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;


/* Protected variables. */
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	UDataTable* PieceDataTable;


/* Protected widgets. */
protected:

	/* The text block displaying the modifier's information. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* ModifierInfo;


};
