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

	/* Default constructor. */
	AMM_LobbyBeaconHostObject();


/* Protected functions. */
protected:

	/* Called when a beacon client is connected to this beacon host object. */
	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;

	/* Called when a client disconnects from this host. */
	virtual void NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor) override;

	/* Destroys the lobby that this beacon is hosting. */
	UFUNCTION(BlueprintCallable)
	void DestroyLobby();

		/* Disconnects all clients currently connected to this lobby. */
		void DisconnectAllClients();
};
