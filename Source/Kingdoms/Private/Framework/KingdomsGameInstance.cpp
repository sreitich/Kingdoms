// Copyright Change Studios, LLC 2023.


#include "Framework/KingdomsGameInstance.h"

#include "SaveGames/UnlockedPieces_SaveGame.h"
#include "SaveGames/ArmyPresets_SaveGame.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UKingdomsGameInstance::UKingdomsGameInstance()
{
}

void UKingdomsGameInstance::CreatePublicServer()
{
	/* Customize the settings for our new public session. */
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 2;

	/* Create a new session using our settings. */
	SessionInterface->CreateSession(0, FName("Kingdoms Session"), SessionSettings);
}

void UKingdomsGameInstance::CreatePrivateServer()
{
	/* Customize the settings for our new private session. */
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings.bShouldAdvertise = false;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPrivateConnections = 2;
	SessionSettings.NumPublicConnections = 0;

	/* Create a new session using our settings. */
	SessionInterface->CreateSession(0, FName("Kingdoms Session"), SessionSettings);
}

void UKingdomsGameInstance::JoinServer()
{
	/* Create a session-search object. */
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	/* Set the settings to use to search for sessions. */
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	/* Search for current sessions. */
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UKingdomsGameInstance::Init()
{
	Super::Init();

	/* Bind our delegates to our game sessions' events. Start by trying to retrieve our online subsystem to access our
	 * session interface. */
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		/* Try to retrieve our session interface from the online subsystem. */
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			/* Bind our functions for when a session is created, found, and joined. */
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UKingdomsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UKingdomsGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UKingdomsGameInstance::OnJoinSessionComplete);
			SessionInterface->OnRegisterPlayersCompleteDelegates.AddUObject(this, &UKingdomsGameInstance::OnRegisterPlayersComplete);
			SessionInterface->OnStartSessionCompleteDelegates.AddUObject(this, &UKingdomsGameInstance::OnStartSessionComplete);
		}
	}

	/* If a save slot exists for unlocked pieces... */
	if (UnlockedPieces_SaveGame && UGameplayStatics::DoesSaveGameExist(UnlockedPieces_SaveGame->SaveSlotName, UnlockedPieces_SaveGame->UserIndex))
	{
		/* Load the save slot into the save game object. */
		UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::LoadGameFromSlot(UnlockedPieces_SaveGame->SaveSlotName, UnlockedPieces_SaveGame->UserIndex));
	}
	/* If a save slot hasn't yet been created for unlocked pieces... */
	else
	{
		/* Create a new save slot and load it into the save game object. */
		UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::CreateSaveGameObject(UUnlockedPieces_SaveGame::StaticClass()));
	}
	
	/* If a save slot exists for army presets... */
	if (ArmyPresets_SaveGame && UGameplayStatics::DoesSaveGameExist(ArmyPresets_SaveGame->SaveSlotName, ArmyPresets_SaveGame->UserIndex))
	{
		/* Load the save slot into the save game object. */
		ArmyPresets_SaveGame = Cast<UArmyPresets_SaveGame>(UGameplayStatics::LoadGameFromSlot(ArmyPresets_SaveGame->SaveSlotName, ArmyPresets_SaveGame->UserIndex));
	}
	/* If a save slot hasn't yet been created for army presets... */
	else
	{
		/* Create a new save slot and load it into the save game object. */
		ArmyPresets_SaveGame = Cast<UArmyPresets_SaveGame>(UGameplayStatics::CreateSaveGameObject(UArmyPresets_SaveGame::StaticClass()));
	}
}

void UKingdomsGameInstance::OnCreateSessionComplete(FName SessionName, bool bSucceeded)
{
	UE_LOG(LogTemp, Error, TEXT("Created session."));
	
	/* When we successfully create a session, travel to a new main menu map as the listen server. */
	if (bSucceeded)
	{
		if (const ULocalPlayer* LocalPlayerPtr = GetFirstGamePlayer())
		{
			// SessionInterface->RegisterPlayer(SessionName, *LocalPlayerPtr->GetPreferredUniqueNetId(), false);
			GetWorld()->ServerTravel("/Game/Maps/L_MainMenu?listen");
		}
	}
}

void UKingdomsGameInstance::OnFindSessionComplete(bool bSucceeded)
{
	/* After searching for sessions, if a session was found, join it. */
	if (bSucceeded)
	{
		/* Get every found session as an array. */
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		/* If there were any sessions with open connections found, join the first one. */
		if (SearchResults.Num())
		{
			if (SearchResults[0].Session.NumOpenPublicConnections)
			{
				SessionInterface->JoinSession(0, FName("Kingdoms Session"), SearchResults[0]);
			}
		}
		/* If the player cannot find a session, create a new one. */
		else
		{
			CreatePublicServer();
		}
	}
}

void UKingdomsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Error, TEXT("Joined session."));

	/* When we successfully join a session, travel to the session's map. */
	if (const ULocalPlayer* LocalPlayerPtr = GetFirstGamePlayer())
	{
		/* Register the local player with the session they just joined. */
		// SessionInterface->RegisterPlayer(SessionName, *LocalPlayerPtr->GetPreferredUniqueNetId(), false);

		if (APlayerController* PlayerControllerPtr = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			/* Get the address of the map to join. */
			FString JoinAddress = "";
			SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
			
			/* Travel to the joined session's corresponding map if the map's address was found. */
			if (JoinAddress != "")
			{
				PlayerControllerPtr->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
			}
		}	
	}
}

void UKingdomsGameInstance::OnRegisterPlayersComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId,
	bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("Registered player"));

	// /* Get a pointer to the session that the player registered to. */
	// const FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(SessionName);
	// const ULocalPlayer* LocalPlayerPtr = GetFirstGamePlayer();
	//
	// /* If the registered player is the host, travel to a new main menu map as a listen server. */
	// if (LocalPlayerPtr && CurrentSession->OwningUserId == LocalPlayerPtr->GetPreferredUniqueNetId())
	// {
	// 	// UE_LOG(LogTemp, Error, TEXT("Registered host."));
	// 	GetWorld()->ServerTravel("/Game/Maps/L_MainMenu?listen");
	// }
	// /* If the registered player is not the host, travel to the session's map. */
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Registered client."));
	// 	/* Get the player's controller. */
	// 	if (APlayerController* PlayerControllerPtr = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	// 	{
	// 		/* Get the address of the map to join. */
	// 		FString JoinAddress = "";
	// 		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
	// 		
	// 		/* Travel to the joined session's corresponding map if the map's address was found. */
	// 		if (JoinAddress != "")
	// 		{
	// 			PlayerControllerPtr->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	// 		}
	// 	}	
	// }
}

void UKingdomsGameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("Started the session."));
}
