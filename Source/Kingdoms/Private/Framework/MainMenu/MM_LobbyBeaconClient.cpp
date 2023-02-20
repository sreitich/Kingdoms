// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_LobbyBeaconClient.h"

AMM_LobbyBeaconClient::AMM_LobbyBeaconClient()
{
}

bool AMM_LobbyBeaconClient::ConnectToServer(const FString& Address)
{
	/* Construct a URL to connect to using the given address. */
	FURL ServerURL = FURL(nullptr, *Address, ETravelType::TRAVEL_Absolute);

	/* Set the default port to use. */
	ServerURL.Port = 7787;

	/* Initialize the beacon client connection and attempt to connect to the host beacon at the given address. */
	return InitClient(ServerURL);
}

void AMM_LobbyBeaconClient::OnConnected()
{
	UE_LOG(LogTemp, Error, TEXT("CLIENT: Client successfully connected to host beacon."));
	FConnected.Broadcast(true);
}

void AMM_LobbyBeaconClient::OnFailure()
{
	UE_LOG(LogTemp, Error, TEXT("CLIENT: Client failed to connect to host beacon."));
	FConnected.Broadcast(false);
}

void AMM_LobbyBeaconClient::LeaveLobby()
{
	/* Destroy this beacon client to close its connection to the lobby. */
	DestroyBeacon();
}
