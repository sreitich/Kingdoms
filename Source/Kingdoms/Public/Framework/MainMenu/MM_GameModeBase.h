// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MM_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called when a player enters the map. Starts the game if there are enough players connected. */
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
