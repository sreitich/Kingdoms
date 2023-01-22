// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MM_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_PlayerController : public APlayerController
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_PlayerController();


/* Protected functions. */
protected:

	/* Called when spawned. */
	virtual void BeginPlay() override;
	
};
