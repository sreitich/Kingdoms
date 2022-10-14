// Copyright Samuel Reitich 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_Modifier.generated.h"

class UButton;
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

	/* Called when destroyed. */
	virtual void NativeDestruct() override;

	/* Strengthens the fresnel of this modifier's source piece. */
	UFUNCTION()
	void OnModifierButtonHovered();

	/* Resets the fresnel of this modifier's source piece. */
	UFUNCTION()
	void OnModifierButtonUnhovered();

	/* Highlights the actor responsible for this modifier depending on its class. */
	UFUNCTION()
	void HighlightSource(bool bRemoveHighlight) const;


/* Protected variables. */
protected:

	/* Stores the information of the modifier that this widget represents. */
	UPROPERTY()
	FModifier ModifierInfo;


/* Protected widgets. */
protected:

	/* The text block displaying the modifier's information. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* DisplayedModifierInfo;

	/* Encompasses the entire widget; when hovered, the source piece of this modifier is highlighted. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ModifierButton;

};
