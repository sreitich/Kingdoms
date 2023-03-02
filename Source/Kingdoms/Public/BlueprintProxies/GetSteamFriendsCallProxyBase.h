// Copyright Change Studios, LLC 2023.

#pragma once

#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"

#include "OnlineSubsystemUtils.h"

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "GetSteamFriendsCallProxyBase.generated.h"

/* Called when the online friends interface finishes an attempt to retrieve the local player's friends list. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetSteamFriendsDelegate, const TArray<FSteamFriend>&, Results);

/* Blueprint node for asynchronously retrieving the local player's friends list from Steam. */
UCLASS()
class KINGDOMS_API UGetSteamFriendsCallProxyBase : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

/* Public functions. */
public:

	/* Creates the proxy that is reflected in blueprints as a custom node. */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="InWorldContextObject"), Category="Steam | Friends")
	static UGetSteamFriendsCallProxyBase* GetSteamFriends(UObject* InWorldContextObject, class APlayerController* InPlayerController);

	/* Queries an attempt to retrieve the local player's friend list via the online friends interface. Called when this
	 * proxy is activated (i.e. when the custom blueprint node is called). */
	virtual void Activate() override;

	/* Called by the OnReadFriendsListComplete delegate when the online friends interface finishes attempting to retrieve
	 * the local player's friends list. */
	void OnGetSteamFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);


/* Protected variables. */
protected:

	/* The controller of the player whose friends list we are retrieving. */
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr = nullptr;

	/* The world context object for this function. */
	UObject* WorldContextObject = nullptr;


/* Protected delegates. */
protected:

	/* Delegate triggered when the online friends interface has finished attempting to retrieve the local player's
	 * friends list. */
	FOnReadFriendsListComplete OnReadFriendsListComplete;
	
	/* Delegate triggered when the online friends interface successfully retrieves the local player's friends list. */
	UPROPERTY(BlueprintAssignable)
	FGetSteamFriendsDelegate OnSuccess;
	
	/* Delegate triggered when the online friends interface fails to retrieve the local player's friends list. */
	UPROPERTY(BlueprintAssignable)
	FGetSteamFriendsDelegate OnFailure;

};