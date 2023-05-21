// Copyright Change Studios, LLC 2023.

#pragma once

#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"
#include "UserDefinedData/Match_UserDefinedData.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KingdomsGameInstance.generated.h"

/* Called when the online friends interface successfully retrieves and reads the local player's friends list. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadFriendsListSuccess, const TArray<FSteamFriend>&, FriendsList);
/* Called when the online friends interface fails to retrieve and/or read the local player's friends list. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadFriendsListFail, const TArray<FSteamFriend>&, FriendsList);

/* Called when the online friends interface finishes an attempt to send a game invitation to another player. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInvitationSentDelegate, FSteamFriend /* The player receiving the invitation. */, InviteReceiver);
/* Called when the online friends interface finishes an attempt to accept a game invitation from another player. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInvitationAcceptedDelegate, FSteamFriend /* The player who sent the invitation. */, InviteSender);
/* Called when the online friends interface finishes an attempt to reject a game invitation from another player. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInvitationRejectedDelegate, FSteamFriend /* The player who sent the invitation. */, InviteSender);

class UArmyPresets_SaveGame;
class UUnlockedPieces_SaveGame;
class USaveGame;

UCLASS()
class KINGDOMS_API UKingdomsGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	UKingdomsGameInstance();

	/* Uploads the given save game slot to the Steam cloud. Returns false if the given save game is empty. */
	UFUNCTION(BlueprintCallable)
	bool UploadSaveGameToSteam(FString SaveGameSlotName);

	/* Creates a new public session that anyone can join via matchmaking. */
	UFUNCTION()
	void CreatePublicServer();

	/* Creates a new private session that can only be joined via an invite from the host. */
	UFUNCTION()
	void CreatePrivateServer();

	/* Searches for sessions with presence. */
	UFUNCTION()
	void JoinServer();

	/* Safely leaves the current session. Destroys the session if this is the host. */
	UFUNCTION()
	void LeaveServer(EMatchStatus CurrentMatchStatus);

	/* Provides whether or not the local player is in a session and if they are the host of that session. */
	UFUNCTION()
	void GetCurrentSessionInfo(bool& bInSession, bool& bIsHost) const;

	/* Tries to retrieve the local player's friends list, triggering OnReadFriendsListCompleteDelegate when complete. */
	UFUNCTION(BlueprintCallable, Category="Steam|Friends")
	void ReadSteamFriendsList();

	/* Sends a game invitation to the given player. */
	void SendInviteToPlayer(const FUniqueNetId& PlayerToInvite);

	/* Sends a game invitation to the given player. Blueprint wrapper for SendInviteToPlayer. */
	UFUNCTION(BlueprintCallable, Category="Steam|Friends")
		void B_SendInviteToPlay(FSteamFriend FriendToInvite);

	/* Attempts to accept or reject an invitation from another player. */
	UFUNCTION(BlueprintCallable, Category="Steam|Friends")
		void ReplyToInviteFromPlayer(FSteamFriend FriendToReplyTo, bool bAccept);


/* Public variables. */
public:

	/* References to save game objects to use when saving and loading. */
	UPROPERTY(BlueprintReadWrite, Category = "Save Games")
	UUnlockedPieces_SaveGame* UnlockedPieces_SaveGame;

	UPROPERTY(BlueprintReadWrite, Category = "Save Games")
	UArmyPresets_SaveGame* ArmyPresets_SaveGame;


/* Protected functions. */
protected:

	/* Initialization function. */
	virtual void Init() override;

	/* Sets up the given save game's information. */
	template<class T>
	void SetUpSaveGame(USaveGame** SaveGameReference, FString SlotName);

	/* Called when an attempt to start a new game session has ended. */
	virtual void OnCreateSessionComplete(FName SessionName, bool bSucceeded);

	/* Called when game session search finishes. Joins the first session found or creates a new one if none were
	 * found. */
	virtual void OnFindSessionComplete(bool bSucceeded);

	/* Called when an attempt to join a session has ended. Registers the joined player with the session. */
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/* Attempts to start the session if there are now enough registered players. */
	virtual void OnRegisterPlayersComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccessful);

	/* Sends the connected players to the correct map. */
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	/* Gets the friends list and triggers the corresponding FOnReadFriendsList delegate depending on whether it
	 * succeeded or failed. */
	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	/* Tells the player that an invite was successfully sent to a player. */
	void OnSendInviteComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorString);

	/* Creates an invitation pop-up for the player to accept a received invitation. */
	void OnInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FriendId);

	/* Attempts to join the lobby of the player who sent the invitation, if successful. */
	void OnAcceptInviteComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr);

	/* Communicates to the sender and the receiver that the invitation was rejected. */
	void OnRejectInviteComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr);


/* Protected variables. */
protected:

	/* Persistent pointer to the local online subsystem accessor. */
	IOnlineSubsystem* OnlineSubsystem;

	/* Persistent pointer to the object used to search for sessions. */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	/* Persistent pointer to the local online subsystem's sessions interface accessor.  */
	IOnlineSessionPtr SessionInterface;

	/* Persistent pointer to the local friends interface accessor. */
	IOnlineFriendsPtr FriendsInterface;

	/* Persistent pointer to the Steam remote storage interface accessor. */
	ISteamRemoteStorage* SteamRemoteStorageInterface;

	/* The name of the session that this player is currently in, if they are in one. */
	FName CurrentSessionName;

	/* The name of the lobby that this player is currently in, if they are in one. */
	FName CurrentLobbyName;


/* Protected delegates. */
protected:

	/* Triggered when the online friends interface has finished attempting to retrieve the most recent version of the
	 * local player's friends list. */
	FOnReadFriendsListComplete OnReadFriendsListCompleteDelegate;

		/* Triggered when the online friends interface successfully reads the local player's friends list. */
		UPROPERTY(BlueprintAssignable, Category="Steam|Friends")
		FOnReadFriendsListSuccess OnReadFriendsListSuccessDelegate;

		/* Triggered when the online friends interface fails to read the local player's friends list. */
		UPROPERTY(BlueprintAssignable, Category="Steam|Friends")
		FOnReadFriendsListFail OnReadFriendsListFailDelegate;
	
	/* Triggered when the online friends interface has finished attempting to send an invite to another
	 * player. */
	FOnSendInviteComplete OnSendInviteCompleteDelegate;

		/* Triggered when the online friends interface successfully sends an invite to another player. */
		UPROPERTY(BlueprintAssignable)
		FInvitationSentDelegate OnSendInvitationSuccessDelegate;

		/* Triggered when the online friends interface fails to send an invite to another player. */
		UPROPERTY(BlueprintAssignable)
		FInvitationSentDelegate OnSendInvitationFailureDelegate;


	/* Triggered when the online friends interface finishes attempting to accept an invite from another player. */
	FOnAcceptInviteComplete OnAcceptInviteCompleteDelegate;

		/* Triggered when the online friends interface successfully accepts an invitation from another player. */
		UPROPERTY(BlueprintAssignable)
		FInvitationAcceptedDelegate OnAcceptInvitationSuccessDelegate;

		/* Triggered when the online friends interface fails to accept an invitation from another player. */
		UPROPERTY(BlueprintAssignable)
		FInvitationAcceptedDelegate OnAcceptInvitationFailureDelegate;


		/* Triggered when the online friends interface successfully rejects an invitation from another player. */
		UPROPERTY(BlueprintAssignable)
		FInvitationRejectedDelegate OnRejectInvitationSuccessDelegate;

		/* Triggered when the online friends interface fails to reject an invitation from another player. */
		UPROPERTY(BlueprintAssignable)
		FInvitationRejectedDelegate OnRejectInvitationFailureDelegate;

};
