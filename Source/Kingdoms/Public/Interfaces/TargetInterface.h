// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINGDOMS_API ITargetInterface
{
	GENERATED_BODY()

public:

	/* Highlights a target depending on its class. */
	virtual void HighlightTarget() = 0;

	/* Removes the highlight from a target depending on its class. */
	virtual void RemoveTargetHighlight() = 0;
};
