// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_LobbyBeaconHostObject.h"

#include "Framework/MainMenu/MM_LobbyBeaconClient.h"

AMM_LobbyBeaconHostObject::AMM_LobbyBeaconHostObject()
{
	/* Set which client beacons will connect to this host beacon. The AOnlineBeaconHost will pair beacon clients to
	 * their corresponding host object. */
	ClientBeaconActorClass = AMM_LobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();
}

void AMM_LobbyBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (NewClientActor)
	{
		UE_LOG(LogTemp, Error, TEXT("A"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("B"));
	}
}
