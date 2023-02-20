// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_LobbyBeaconHostObject.h"

#include "Framework/MainMenu/MM_GameModeBase.h"
#include "Framework/MainMenu/MM_LobbyBeaconClient.h"
#include "Framework/MainMenu/MM_LobbyBeaconState.h"

#include "OnlineBeaconHost.h"

AMM_LobbyBeaconHostObject::AMM_LobbyBeaconHostObject()
{
	/* Set which beacon clients will connect to this beacon host object. The AOnlineBeaconHost will pair beacon clients
	 * to their corresponding host object and form a connection. */
	ClientBeaconActorClass = AMM_LobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	/* Set the class to use for the lobby state. */
	LobbyStateClass = AMM_LobbyBeaconState::StaticClass();
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

void AMM_LobbyBeaconHostObject::NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor)
{
	Super::NotifyClientDisconnected(LeavingClientActor);

	UE_LOG(LogTemp, Error, TEXT("HOST: Client has disconnected."))
}

void AMM_LobbyBeaconHostObject::DestroyLobby()
{
	UE_LOG(LogTemp, Error, TEXT("HOST: Lobby destroyed."));
	/* Disconnect all clients connected to this lobby. */
	DisconnectAllClients();

	/* Unregister this lobby and destroy the host beacon. */
	if (const AMM_GameModeBase* GameModePtr = GetWorld()->GetAuthGameMode<AMM_GameModeBase>())
	{
		if (AOnlineBeaconHost* Host = GameModePtr->GetBeaconHost())
		{
			Host->UnregisterHost(BeaconTypeName);
			Host->DestroyBeacon();
		}
	}
}

void AMM_LobbyBeaconHostObject::DisconnectAllClients()
{
	UE_LOG(LogTemp, Error, TEXT("HOST: All clients disconnected."));

	/* Iterate through every connected client and disconnect them from this host. */
	for (AOnlineBeaconClient* Client : ClientActors)
	{
		if (IsValid(Client))
		{
			DisconnectClient(Client);
		}
	}
}
