// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KingdomsGameInstance.generated.h"

UCLASS()
class KINGDOMS_API UKingdomsGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	UKingdomsGameInstance();

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


/* Public variables. */
public:

	/* References to save game objects to use when saving and loading. */
	UPROPERTY()
	class UUnlockedPieces_SaveGame* UnlockedPieces_SaveGame;

	UPROPERTY()
	class UArmyPresets_SaveGame* ArmyPresets_SaveGame;


/* Protected functions. */
protected:

	/* Initialization function. */
	virtual void Init() override;

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


/* Protected variables. */
protected:

	/* Persistent pointer to the game instance's online subsystem. */
	IOnlineSubsystem* OnlineSubsystem;

	/* Persistent pointer to the object used to search for sessions. */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	/* Persistent pointer to the object used to use the online subsystem's sessions interface.  */
	IOnlineSessionPtr SessionInterface;

	/* The name of the session that this player is currently in, if they are in one. */
	FName CurrentSessionName;

	/* The name of the lobby that this player is currently in, if they are in one. */
	FName CurrentLobbyName;

};
