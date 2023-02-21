// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "LobbyBeaconClient.h"
#include "MM_LobbyBeaconClient.generated.h"

/* Fires when a client successfully or unsuccessfully connects to a host. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectSuccess, bool, bConnectionSuccessful);
/* Fires when a client is disconnected by a host. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnected);

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

};
