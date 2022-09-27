// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModifierBoardPopup.generated.h"

UCLASS()
class KINGDOMS_API AModifierBoardPopup : public AActor
{
	GENERATED_BODY()

/* Public functions. */
public:	

	/* Updates, displays, and animates this modifier pop-up. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_ActivateModifierPopUp(FVector Location, int ValueChange, bool bStrength, float Duration);

		/* Reverses the pop-up animation and destroys this pop-up. */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void BP_DestroyModifierPopUp();

};
