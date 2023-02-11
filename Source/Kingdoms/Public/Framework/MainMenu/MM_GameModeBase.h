// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MM_GameModeBase.generated.h"

class AMM_LobbyBeaconHostObject;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

/* Public functions. */
public:

	AMM_GameModeBase();

	/* Called when a player enters the map. Starts the game if there are enough players connected. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/* Creates a new host beacon to host a lobby. */
	UFUNCTION()
	bool CreateHostBeacon();


protected:

	AMM_LobbyBeaconHostObject* HostObject;

};
