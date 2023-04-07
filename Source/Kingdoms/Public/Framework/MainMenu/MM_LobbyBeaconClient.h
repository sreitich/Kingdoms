// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Menu_UserDefinedData.h"

#include "CoreMinimal.h"
#include "LobbyBeaconClient.h"
#include "MM_LobbyBeaconClient.generated.h"

class AMM_LobbyBeaconClient;

/* Fires when a client successfully or unsuccessfully connects to a host. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectSuccess, bool, bConnectionSuccessful);
/* Fires when a client is disconnected by a host. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnected);
/* Fires when the lobby's information changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLobbyUpdated, FCustomLobbyData, UpdatedLobbyInfo);

UCLASS()
class KINGDOMS_API AMM_LobbyBeaconClient : public ALobbyBeaconClient
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_LobbyBeaconClient();

	/* Called when the client is disconnected by the host. */
	UFUNCTION(Client, Reliable)
	virtual void Client_OnDisconnected();

	/* Called when the lobby information is changed to update the client's information. */
	UFUNCTION(Client, Reliable)
	void Client_OnLobbyUpdated(FCustomLobbyData NewLobbyInfo);

	/* Getter for PlayerIndex. */
	FORCEINLINE uint8 GetPlayerIndex() const { return PlayerIndex; }

	/* Setter for PlayerIndex. */
	FORCEINLINE void SetPlayerIndex(uint8 NewPlayerIndex) { PlayerIndex = NewPlayerIndex; };


/* Protected functions. */
protected:

	/* Attempts to connect this beacon client to the beacon host at the given address. */
	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(const FString& Address);

	/* Called when this beacon client successfully connects to a beacon host. */
	virtual void OnConnected() override;

	/* Called when this beacon client fails to connect to a beacon host. */
	virtual void OnFailure() override;

	/* Disconnects this beacon client from the host. */
	UFUNCTION(BlueprintCallable)
	void LeaveLobby();


/* Protected variables. */
protected:

    /* OnConnected delegate. */
    UPROPERTY(BlueprintAssignable)
    FConnectSuccess FOnConnected;

	/* OnDisconnected delegate. */
	UPROPERTY(BlueprintAssignable)
	FDisconnected FOnDisconnected;

	/* OnLobbyUpdated delegate. */
	UPROPERTY(BlueprintAssignable)
	FLobbyUpdated FOnLobbyUpdated;

	/* The client's local copy of the host's lobby info. This is used to temporarily store the lobby info at the time
	 * of connection, since the client has no way of telling the server when it has navigated to the lobby menu and is
	 * ready to update the lobby information for the first time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lobby Information")
	FCustomLobbyData LocalLobbyInfo = FCustomLobbyData();

	/* The player's index in the lobby it is currently connected to. */
	uint8 PlayerIndex;

};
