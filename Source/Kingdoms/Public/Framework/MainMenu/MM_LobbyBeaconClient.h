// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "LobbyBeaconClient.h"
#include "MM_LobbyBeaconClient.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_LobbyBeaconClient : public ALobbyBeaconClient
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_LobbyBeaconClient();


/* Protected functions. */
protected:

	/* Attempts to connect this beacon client to the beacon host at the given address. */
	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(const FString& Address);

	/* Called when this beacon client successfully connects to a beacon host. */
	virtual void OnConnected() override;

	/* Called when this beacon client fails to connect to a beacon host. */
	virtual void OnFailure() override;

};
