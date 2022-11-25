// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_TurnProgressWidget.generated.h"

class UImage;
class UTexture2D;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_TurnProgressWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates an action indicator  */
	UFUNCTION()
	void UpdateActionIndicator(bool bUsed, bool bMoveAction) const;


/* Protected widgets. */
protected:

	/* Indicates whether or not the move action has been used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UImage* MoveActionIndicator;

	/* Indicates whether or not the ability action has been used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UImage* AbilityActionIndicator;


/* Protected textures. */
protected:

	/* Indicates that the player has not used a move action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Textures")
	UTexture2D* MoveUnusedImage;

	/* Indicates that the player has used a move action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Textures")
	UTexture2D* MoveUsedImage;

	/* Indicates that the player has not used an ability action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Textures")
	UTexture2D* AbilityUnusedImage;

	/* Indicates that the player has used an ability action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Textures")
	UTexture2D* AbilityUsedImage;
	
};
