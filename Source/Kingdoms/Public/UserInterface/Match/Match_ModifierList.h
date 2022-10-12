// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_ModifierList.generated.h"

class UOverlay;
class UScrollBox;

class UMatch_Modifier;

struct FModifier;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_ModifierList : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	UFUNCTION(BlueprintCallable)
	void PopulateModifierList(TArray<FModifier> Modifiers);


/* Public widgets. */
public:

	/* The parent overlay of this widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UOverlay* ModifierListOverlay;

/* Protected widgets. */
protected:

	/* The box that modifiers are added to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UScrollBox* ModifierListBox;


/* Protected widget class-types. */
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Class-Types")
	TSubclassOf<UMatch_Modifier> ModifierClass;

};
