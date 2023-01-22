// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuTransitionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuTransitionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINGDOMS_API IMenuTransitionInterface
{
	GENERATED_BODY()

public:

	/* Should play the deactivation animation for the current widget, which should, in turn, delete the widget and
	 * transition to the next menu with the main menu HUD when it finishes playing. */
	virtual void DeactivateWidget() = 0;

};
