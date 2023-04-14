// Copyright Change Studios, LLC 2023.


#include "Framework/KingdomsGameInstance.h"

#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"
#include "SaveGames/UnlockedPieces_SaveGame.h"
#include "SaveGames/ArmyPresets_SaveGame.h"

#include "Engine/World.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "steam/steam_api.h"
#include "SteamCore/SteamUtilities.h"
#include "UserInterface/MainMenu/MM_HUD.h"
#include "SteamCore/Public/SteamRemoteStorage/SteamRemoteStorage.h"

/* We are always going to be retrieving the default friends list. */
#define DEFAULT_FRIENDS_LIST EFriendsLists::ToString(EFriendsLists::Default)

UKingdomsGameInstance::UKingdomsGameInstance()
{
	/* Bind OnReadFriendsListComplete to be called when the OnReadFriendsListCompleteDelegate delegate fires. */
	OnReadFriendsListCompleteDelegate.BindUObject(this, &UKingdomsGameInstance::OnReadFriendsListComplete);
	/* Bind OnSendInviteComplete to be called when the OnSendInviteCompleteDelegate delegate fires. */
	OnSendInviteCompleteDelegate.BindUObject(this, &UKingdomsGameInstance::OnSendInviteComplete);
	/* Bind OnAcceptInviteComplete to be called when the OnAcceptInviteCompleteDelegate delegate fires. */
	OnAcceptInviteCompleteDelegate.BindUObject(this, &UKingdomsGameInstance::OnAcceptInviteComplete);
}

bool UKingdomsGameInstance::UploadSaveGameToSteam(FString SaveGameSlotName)
{
	/* Dynamically get this save game's file name. */
	const char* FileName = TCHAR_TO_UTF8(*(SaveGameSlotName + ".sav"));

	/* Get the local path of the save file to write to the Steam cloud. */
	const FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + FileName;

	/* Get the local file's data. */
	TArray<uint8> FileData = USteamUtilities::ReadFileToBytes(FilePath);

	/* Don't upload empty save game files. */
	if (FileData.Num() == 0)
	{
		return false;
	}

	/* Upload the data to the Steam cloud. */
	return URemoteStorage::FileWrite(FileName, FileData);
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
	// Change the session name to use the player's ID so sessions aren't repeated
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

void UKingdomsGameInstance::LeaveServer(EMatchStatus CurrentMatchStatus)
{
	/* We don't need any kind of end-of-match code when the player cancels matchmaking. */
	if (CurrentMatchStatus == E_Matchmaking)
	{
		/* Destroy the current session. */
		SessionInterface->DestroySession(CurrentSessionName);

		/* Destroy the displayed queue timer widget. */
		if (AMM_HUD* HUDPtr = GetFirstLocalPlayerController(GetWorld())->GetHUD<AMM_HUD>())
		{
			HUDPtr->CreateQueueTimerWidget(true);
		}
	}
}

void UKingdomsGameInstance::GetCurrentSessionInfo(bool& bInSession, bool& bIsHost) const
{
	/* Try to find the current session if there is a current session name. */
	if (CurrentSessionName != "")
	{
		const FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(CurrentSessionName);

		/* If the current session can be found, then the local player is in a session.*/
		if (CurrentSession)
		{
			bInSession = true;

			/* If the session's owner ID is the local player's ID, then the local player is the host of their current
			 * session. */
			if (CurrentSession->OwningUserId == GetFirstGamePlayer()->GetPreferredUniqueNetId())
			{
				bIsHost = true;
			}
			else
			{
				bIsHost = false;
			}

			return;
		}
	}

	/* If the current session cannot be found, then the player is not in a session, and therefore cannot be a session
	host. */
	bInSession = false;
	bIsHost = false;
}

void UKingdomsGameInstance::ReadSteamFriendsList()
{
	/* Attempt to asynchronously read the local player's friends list if the friends interface is valid. */
	if (FriendsInterface.IsValid())
	{
		/* OnReadFriendsListCompleteDelegate is fired when this process finishes, regardless of its success. */
		FriendsInterface->ReadFriendsList(0, DEFAULT_FRIENDS_LIST, OnReadFriendsListCompleteDelegate);
	}
}

void UKingdomsGameInstance::SendInviteToPlayer(const FUniqueNetId& PlayerToInvite)
{
	if (SteamAPI_Init())
	{
		uint64 FriendID = *((uint64*)PlayerToInvite.GetBytes());
		const char* ConnectionString = TCHAR_TO_ANSI(*FString());

		UE_LOG(LogTemp, Error, TEXT("FriendID: %i"), FriendID);

		UE_LOG(LogTemp, Error, TEXT("Public address: %s"), *ConnectionString);

		// if (ConnectionString)
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Connection string: %s"), *ConnectionString);
		// }
		// UE_LOG(LogTemp, Error, TEXT("FriendID: %i, address: %s, connection string: %s"), FriendID, *Address, *ConnectionString);
		// bool bSuccess = SteamFriends()->InviteUserToGame(FriendID, ConnectionString);
		// UE_LOG(LogTemp, Error, TEXT("%s"), *(bSuccess ? "Successfuly sent invite" : "Failed to send invite"));
	}
}

void UKingdomsGameInstance::B_SendInviteToPlay(FSteamFriend FriendToInvite)
{
	/* Unique net IDs are only exposed in C++, so we need to use a wrapper to get the friend's ID to send the
	 * invitation to. */
	SendInviteToPlayer(*FriendToInvite.UniqueNetID);
}

void UKingdomsGameInstance::ReplyToInviteFromPlayer(FSteamFriend FriendToReplyTo, bool bAccept)
{
	/* Ensure the friends interface is valid. */
	if (FriendsInterface.IsValid())
	{
		/* Attempt to accept the invitation if requested. */
		if (bAccept)
			FriendsInterface->AcceptInvite(0, *FriendToReplyTo.UniqueNetID, DEFAULT_FRIENDS_LIST, OnAcceptInviteCompleteDelegate);
		/* Attempt to reject the invitation if requested. */
		else
			FriendsInterface->RejectInvite(0, *FriendToReplyTo.UniqueNetID, DEFAULT_FRIENDS_LIST);
	}
}

void UKingdomsGameInstance::Init()
{
	Super::Init();

	/* Retrieve our online subsystem for future use. */
	OnlineSubsystem = IOnlineSubsystem::Get();

	/* Bind our delegates to our game sessions' events. */
	if (OnlineSubsystem)
	{
		/* Try to retrieve the session interface from the online subsystem. */
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			/* Bind the functions for asynchronous events. */
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this, &UKingdomsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
				this, &UKingdomsGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
				this, &UKingdomsGameInstance::OnJoinSessionComplete);
			SessionInterface->OnRegisterPlayersCompleteDelegates.AddUObject(
				this, &UKingdomsGameInstance::OnRegisterPlayersComplete);
			SessionInterface->OnStartSessionCompleteDelegates.AddUObject(
				this, &UKingdomsGameInstance::OnStartSessionComplete);
		}

		/* Try to retrieve the friends interface from the online subsystem. */
		FriendsInterface = OnlineSubsystem->GetFriendsInterface();
		if (FriendsInterface.IsValid())
		{
			/* Bind the functions for asynchronous events. */
			FriendsInterface->OnInviteReceivedDelegates.AddUObject(this, &UKingdomsGameInstance::OnInviteReceived);
			FriendsInterface->OnRejectInviteCompleteDelegates->AddUObject(this, &UKingdomsGameInstance::OnRejectInviteComplete);
		}
	}



	/* Retrieve the Steam remote storage interface accessor. */
	SteamRemoteStorageInterface = SteamRemoteStorage();

	/* If there is an unlocked pieces save game file on the Steam cloud, load it. If not, try to get, load, and upload
	 * the local one. If there is not a local save game file, create one, load it, save it, and upload it to the Steam
	 * cloud. */
	if (SteamRemoteStorageInterface)
	{
		/* Dynamically get this save game's file name. */
		const char* UnlockedPiecesSaveFileName = TCHAR_TO_UTF8(*(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName() + ".sav"));

		/* Check if the file exists on the Steam cloud. */
		if (SteamRemoteStorageInterface->FileExists(UnlockedPiecesSaveFileName))
		{
			/* Create a buffer to load the data into using the file's size. */
			const int FileSize = SteamRemoteStorageInterface->GetFileSize(UnlockedPiecesSaveFileName);
			uint8* FileDataBuffer = new uint8[FileSize];

			/* Load the file's data into the buffer. */
			if (SteamRemoteStorageInterface->FileRead(UnlockedPiecesSaveFileName, FileDataBuffer, FileSize))
			{
				/* Get the local path of the save file to write the Steam cloud data into. */
				const FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + UnlockedPiecesSaveFileName;

				/* Convert the buffer's data into a TArray to use WriteBytesToFile. */
				TArray<uint8> FileDataArr;
				FileDataArr.Append(FileDataBuffer, FileSize);

				/* Write the Steam cloud data into the local save file. */
				const bool bWriteSuccessful = USteamUtilities::WriteBytesToFile(true, FilePath, FileDataArr);

				/* If the file was written successfully and a save game file exists, load it. */
				if (bWriteSuccessful && UGameplayStatics::DoesSaveGameExist(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0))
				{
					UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::LoadGameFromSlot(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0));
				}
			}
		}
		/* If the save game file does not exist on the Steam cloud, it hasn't been uploaded yet. Try to get and upload the
		 * local save game file if it exists. If not, create one and upload it. */
		else
		{
			/* If the save slot exists, try to get it and upload it to the Steam cloud. */
			if (UGameplayStatics::DoesSaveGameExist(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0))
			{
				/* Load the save game. */
				UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::LoadGameFromSlot(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0));

				/* Upload the save game to the Steam cloud. */
				if (UnlockedPieces_SaveGame)
				{
					UploadSaveGameToSteam(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName());
				}
			}
			/* If a save slot does not exist, it hasn't been created locally either. Create it, save it, and upload it to the Steam cloud. */
			else
			{
				/* Create a new save file and load it. */
				UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::CreateSaveGameObject(UUnlockedPieces_SaveGame::StaticClass()));

				/* Save the save game locally and upload it to the Steam cloud. */
				if (UnlockedPieces_SaveGame)
				{
					UGameplayStatics::SaveGameToSlot(UnlockedPieces_SaveGame, UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0);

					UploadSaveGameToSteam(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName());
				}
			}
		}
	}
	
}

void UKingdomsGameInstance::OnCreateSessionComplete(FName SessionName, bool bSucceeded)
{
	UE_LOG(LogTemp, Error, TEXT("Created session."));

	/* When we successfully create a session, save the name of the session and travel to a new main menu map as the
	 * listen server. */
	if (bSucceeded)
	{
		if (const ULocalPlayer* LocalPlayerPtr = GetFirstGamePlayer())
		{
			/* Save the name of the session that the player just created. */
			CurrentSessionName = SessionName;
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

	/* When we successfully join a session, save a reference to the session and travel to the session's map. */
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		/* Save the name of the session that the player just joined. */
		CurrentSessionName = SessionName;

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

void UKingdomsGameInstance::OnRegisterPlayersComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccessful)
{
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

void UKingdomsGameInstance::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	/* If the friends list could be read, broadcast that is was successfully read, passing the list of friends as an
	 * array of FSteamFriend structures. */
	if (bWasSuccessful)
	{
		if (FriendsInterface.IsValid())
		{
			/* Get the friends list. */
			TArray<TSharedRef<FOnlineFriend>> OnlineFriends;
			FriendsInterface->GetFriendsList(0, ListName, OnlineFriends);

			/* Convert every friend in the list into an FSteamFriend. */
			TArray<FSteamFriend> FriendsList = TArray<FSteamFriend>();
			for (TSharedRef<FOnlineFriend> Friend : OnlineFriends)
			{
				FSteamFriend SteamFriend = FSteamFriend(Friend);
				FriendsList.Add(SteamFriend);
			}

			/* Sort the friends list. */
			FriendsList.Sort();

			/* Broadcast that the list was successfully read, passing in the list as our new array of FSteamFriends. */
			OnReadFriendsListSuccessDelegate.Broadcast(FriendsList);
		}
	}
	/* If the friends list could not be read, broadcast that it failed to be read, passing an empty array. */
	else
	{
		TArray<FSteamFriend> FriendsList = TArray<FSteamFriend>();
		OnReadFriendsListFailDelegate.Broadcast(FriendsList);
	}
}

void UKingdomsGameInstance::OnSendInviteComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorString)
{
	UE_LOG(LogTemp, Error, TEXT("Invitation sent to %s, and result was %i"), *FriendId.ToString(), bWasSuccessful);
	/* Ensure the friends interface is valid. */
	if (FriendsInterface.IsValid())
	{
		/* Get the friend whom we're sending an invitation to from their net ID. */
		TSharedPtr<FOnlineFriend> Friend = FriendsInterface->GetFriend(0, FriendId, ListName);
		FSteamFriend SteamFriend = FSteamFriend(Friend);
	
		/* Broadcast whether or not the invitation to the given player was successfully sent. */
		if (bWasSuccessful)
			OnSendInvitationSuccessDelegate.Broadcast(Friend);
		else
			OnSendInvitationFailureDelegate.Broadcast(Friend);
	}
}

void UKingdomsGameInstance::OnInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FriendId)
{
	if (FriendsInterface.IsValid())
	{
		TSharedPtr<FOnlineFriend> Sender = FriendsInterface->GetFriend(0, FriendId, DEFAULT_FRIENDS_LIST);
		if (Sender.IsValid())
		{
			FSteamFriend Friend(Sender);

			AMM_HUD* HUDPtr = Cast<AMM_HUD>(GetFirstLocalPlayerController(GetWorld())->GetHUD());
			if (IsValid(HUDPtr))
			{
				HUDPtr->CreateInvitationPopUpWidget(false, Friend);
			}
		}
	}
}

void UKingdomsGameInstance::OnAcceptInviteComplete(int32 LocalUserNum, bool bWasSuccessful,
	const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr)
{
	/* Ensure the friends interface is valid. */
	if (FriendsInterface.IsValid())
	{
		/* Get the friend whose invitation we're accepting from their net ID. */
		TSharedPtr<FOnlineFriend> Friend = FriendsInterface->GetFriend(0, FriendId, ListName);
		FSteamFriend SteamFriend = FSteamFriend(Friend);

		/* Broadcast the result of the invitation acceptance attempt. */
		if (bWasSuccessful)
			OnAcceptInvitationSuccessDelegate.Broadcast(Friend);
		else
			OnAcceptInvitationFailureDelegate.Broadcast(Friend);
	}
}

void UKingdomsGameInstance::OnRejectInviteComplete(int32 LocalUserNum, bool bWasSuccessful,
	const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr)
{
	/* Ensure the friends interface is valid. */
	if (FriendsInterface.IsValid())
	{
		/* Get the friend whose invitation we're rejecting from their net ID. */
		TSharedPtr<FOnlineFriend> Friend = FriendsInterface->GetFriend(0, FriendId, ListName);
		FSteamFriend SteamFriend = FSteamFriend(Friend);
	
		/* Broadcast the result of the invitation rejection attempt. */
		if (bWasSuccessful)
			OnRejectInvitationSuccessDelegate.Broadcast(Friend);
		else
			OnRejectInvitationFailureDelegate.Broadcast(Friend);
	}
}
