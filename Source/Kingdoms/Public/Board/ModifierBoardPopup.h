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


/* Protected functions. */
protected:

	/* Called when this pop-up finishes playing. Used to play queued pop-ups so that multiple ones don't play at the same time. */
	UFUNCTION(BlueprintCallable)
	void OnModifierPopUpFinished();
};
