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


/* Public variables. */
public:

	/* References to save game objects to use when saving and loading. */
	UPROPERTY()
	class UUnlockedPieces_SaveGame* UnlockedPieces_SaveGame;

	UPROPERTY()
	class UArmyPresets_SaveGame* ArmyPresets_SaveGame;


/* Protected functions. */
protected:

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool bSucceeded);

	virtual void OnFindSessionComplete(bool bSucceeded);

	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer();

	UFUNCTION(BlueprintCallable)
	void JoinServer();


/* Protected variables. */
protected:

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;
	
};
