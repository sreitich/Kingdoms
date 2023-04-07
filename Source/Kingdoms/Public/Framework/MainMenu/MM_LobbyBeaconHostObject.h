// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Menu_UserDefinedData.h"

#include "CoreMinimal.h"
#include "LobbyBeaconHost.h"
#include "MM_LobbyBeaconHostObject.generated.h"

/* Called when the host updates the lobby information. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostLobbyUpdated, FCustomLobbyData, UpdatedLobbyInfo);

UCLASS()
class KINGDOMS_API AMM_LobbyBeaconHostObject : public ALobbyBeaconHost
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_LobbyBeaconHostObject();

	/* Local getter for LobbyInfo. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FCustomLobbyData GetLobbyInfo() const { return LobbyInfo; }

	/* Setter for LobbyInfo. This does not affect the player names. */
	UFUNCTION(BlueprintCallable)
	void UpdateLobbyInfo(FCustomLobbyData NewLobbyInfo);


/* Protected functions. */
protected:

	/* Called when this actor is spawned. */
	virtual void BeginPlay() override;

	/* Called when a beacon client is connected to this beacon host object. */
	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;

	/* Called when a client disconnects from this host. */
	virtual void NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor) override;

    /* Called to disconnect a given client from this host. */
	virtual void DisconnectClient(AOnlineBeaconClient* ClientActor) override;

	/* Destroys the lobby that this beacon is hosting. */
	UFUNCTION(BlueprintCallable)
	void DestroyLobby();

		/* Disconnects all clients currently connected to this lobby. */
		void DisconnectAllClients();

	/* Replicates the given lobby information to all connected clients. */
	void UpdateClientLobbyInfo(FCustomLobbyData NewLobbyInfo);


/* Protected variables. */
protected:

	/* The settings and information of this custom lobby. */
	UPROPERTY(EditDefaultsOnly, Category="Lobby Information")
	FCustomLobbyData LobbyInfo = FCustomLobbyData();

	/* OnHostLobbyUpdated delegate. */
	UPROPERTY(BlueprintAssignable)
	FHostLobbyUpdated FOnHostLobbyUpdated;

};
