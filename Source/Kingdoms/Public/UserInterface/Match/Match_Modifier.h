// Copyright Samuel Reitich 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_Modifier.generated.h"

class UButton;
class UMatch_ModifierList;
class URichTextBlock;

class UDataTable;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_Modifier : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates the text displayed by this modifier widget and the piece that it highlights when hovered. */
	UFUNCTION(BlueprintCallable)
	void UpdateDisplayedModifier(UMatch_ModifierList* InParentModifierList, FModifier NewModifier, bool bAlignedLeft);


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

	/* The list containing this modifier entry. */
	UPROPERTY()
	UMatch_ModifierList* ParentModifierList;

	/* Stores the information of the modifier that this widget represents. */
	UPROPERTY()
	FModifier ModifierInfo;

	/* The play-rate for highlighting or un-highlighting a source piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Piece Info")
	float HighlightSourceRate = 0.5f;


/* Protected widgets. */
protected:

	/* The text block displaying the modifier's information. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* DisplayedModifierInfo;

	/* Encompasses the entire widget; when hovered, the source piece of this modifier is highlighted. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ModifierButton;

};
