// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_LobbyBeaconHostObject.h"

#include "Framework/MainMenu/MM_LobbyBeaconClient.h"

AMM_LobbyBeaconHostObject::AMM_LobbyBeaconHostObject()
{
	/* Set which beacon clients will connect to this beacon host object. The AOnlineBeaconHost will pair beacon clients
	 * to their corresponding host object and form a connection. */
	ClientBeaconActorClass = AMM_LobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();
}

void AMM_LobbyBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (NewClientActor)
	{
		UE_LOG(LogTemp, Error, TEXT("HOST: Host beacon connected to client."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HOST: Host beacon failed to connect to client."));
	}
}
