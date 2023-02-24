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

void AMM_LobbyBeaconHostObject::UpdateLobbyInfo(FCustomLobbyInformation NewLobbyInfo)
{
	LobbyInfo.Map = NewLobbyInfo.Map;
	/* Don't update the player list from blueprints. */

	/* Update the lobby information for all connected clients. */
	UpdateClientLobbyInfo(NewLobbyInfo);

	/* Update the lobby information for the host locally. */
	FOnHostLobbyUpdated.Broadcast(LobbyInfo);
}

void AMM_LobbyBeaconHostObject::BeginPlay()
{
	Super::BeginPlay();

	// This will be changed to the actual player's name later.
	/* Add the host to the list of player names to display. */
	LobbyInfo.PlayerNames.Add(FString("Host"));
}

void AMM_LobbyBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	/* Initialize the lobby information on the client if the connection was successful. */
	if (NewClientActor)
	{
		// This will be changed to the player's actual name later.
		/* Add the connected player to the list of player names to display. */
		FString PlayerName = FString("Player ");
		const uint8 PlayerIndex = LobbyInfo.PlayerNames.Num();
		PlayerName.Append(FString::FromInt(PlayerIndex));
		LobbyInfo.PlayerNames.Add(PlayerName);

		/* Set the new client's player index. */
		if (AMM_LobbyBeaconClient* Client = Cast<AMM_LobbyBeaconClient>(NewClientActor))
			Client->SetPlayerIndex(PlayerIndex);

		/* Update the displayed lobby info for the host locally. */
		FOnHostLobbyUpdated.Broadcast(LobbyInfo);
		/* Update all connected clients' lobby info. */
		UpdateClientLobbyInfo(LobbyInfo);

		UE_LOG(LogTemp, Error, TEXT("HOST: Host beacon connected to clients."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HOST: Host beacon failed to connect to client."));
	}
}

void AMM_LobbyBeaconHostObject::NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor)
{
	Super::NotifyClientDisconnected(LeavingClientActor);

	/* Get every client whose player index is after the leaving client's index and decrement their indices. This allows
	 * the list of player names to maintain accurate indices of each player's name. */
	if (AMM_LobbyBeaconClient* LeavingClient = Cast<AMM_LobbyBeaconClient>(LeavingClientActor))
	{
		uint8 LeavingPlayerIndex = LeavingClient->GetPlayerIndex();

		for (AOnlineBeaconClient* ClientActor : ClientActors)
        {
        	if (AMM_LobbyBeaconClient* Client = Cast<AMM_LobbyBeaconClient>(ClientActor))
        	{
        		if (Client->GetPlayerIndex() > LeavingPlayerIndex)
        		{
        			Client->SetPlayerIndex(Client->GetPlayerIndex() - 1);
        		}
        	}
        }

		/* Remove the disconnected client from the player list. */
		LobbyInfo.PlayerNames.RemoveAt(LeavingPlayerIndex);
	}

	/* Update the displayed lobby info for the host locally. */
	FOnHostLobbyUpdated.Broadcast(LobbyInfo);
	/* Update all connected clients' lobby info. */
	UpdateClientLobbyInfo(LobbyInfo);

	UE_LOG(LogTemp, Error, TEXT("HOST: Client has disconnected."))
}

void AMM_LobbyBeaconHostObject::DisconnectClient(AOnlineBeaconClient* ClientActor)
{
	/* To disconnect a client, trigger its OnDisconnected function and disconnect it form the host. */
	AOnlineBeaconHost* BeaconHost = Cast<AOnlineBeaconHost>(GetOwner());
	if (BeaconHost)
	{
		/* Call the client's OnDisconnected function. */
		if (AMM_LobbyBeaconClient* Client = Cast<AMM_LobbyBeaconClient>(ClientActor))
		{
			Client->Client_OnDisconnected();
		}

		/* Disconnect the client from the host. */
		BeaconHost->DisconnectClient(ClientActor);
	}
}

void AMM_LobbyBeaconHostObject::DestroyLobby()
{
	UE_LOG(LogTemp, Error, TEXT("HOST: Lobby destroyed."));
	/* Disconnect all clients connected to this lobby. */
	DisconnectAllClients();

	/* Unregister this lobby and destroy the host beacon. */
	if (AOnlineBeaconHost* Host = Cast<AOnlineBeaconHost>(GetOwner()))
	{
		Host->UnregisterHost(BeaconTypeName);
		Host->DestroyBeacon();
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

void AMM_LobbyBeaconHostObject::UpdateClientLobbyInfo(FCustomLobbyInformation NewLobbyInfo)
{
	/* Update the lobby information for all connected clients. */
	for (AOnlineBeaconClient* Client : ClientActors)
	{
		if (AMM_LobbyBeaconClient* LobbyClient = Cast<AMM_LobbyBeaconClient>(Client))
		{
			LobbyClient->Client_OnLobbyUpdated(LobbyInfo);
		}
	}
}
