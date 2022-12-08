// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModifierBoardPopup.generated.h"

class AParentPiece;

UCLASS()
class KINGDOMS_API AModifierBoardPopup : public AActor
{
	GENERATED_BODY()

/* Public functions. */
public:	

	/* Updates, displays, and animates this modifier pop-up at the given location. If a valid actor is passed in and
	 * bFollowActor is true, the pop-up will follow the actor's pop-up location component. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_ActivateModifierPopUp(AActor* BaseActor, FVector Location, int ValueChange, bool bStrength, float Duration, bool bFollowActor);


/* Protected functions. */
protected:

	/* Called when this pop-up finishes playing. Used to play queued pop-ups so that multiple ones don't play at the same time. */
	UFUNCTION(BlueprintCallable)
	void OnModifierPopUpFinished();
};
