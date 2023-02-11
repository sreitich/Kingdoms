// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "LobbyBeaconHost.h"
#include "MM_LobbyBeaconHostObject.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_LobbyBeaconHostObject : public ALobbyBeaconHost
{
	GENERATED_BODY()

/* Public functions. */
public:

	AMM_LobbyBeaconHostObject();


/* Protected functions. */
protected:

	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
	
};
