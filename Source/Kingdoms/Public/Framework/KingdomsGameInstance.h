// Copyright Change Studios, LLC 2023.

#pragma once

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KingdomsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UKingdomsGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	UKingdomsGameInstance();

	/* Creates a new session. */
	UFUNCTION()
	void CreateServer(bool bFriendServer);

	/* Attempts to join an ongoing session. Creates a new session if one cannot be found. */
	UFUNCTION()
	void JoinServer();


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

	/* Called when game session search finishes. */
	virtual void OnFindSessionComplete(bool bSucceeded);

	/* Called when an attempt to join a session has ended. */
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


/* Protected variables. */
protected:

	/* Persistent pointer to the object used to search for sessions. */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	/* Persistent pointer to the object used to interface with the online subsystem's sessions.  */
	IOnlineSessionPtr SessionInterface;
	
};
