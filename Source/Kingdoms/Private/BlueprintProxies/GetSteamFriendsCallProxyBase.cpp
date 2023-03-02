// Copyright Change Studios, LLC 2023.


#include "BlueprintProxies/GetSteamFriendsCallProxyBase.h"

UGetSteamFriendsCallProxyBase::UGetSteamFriendsCallProxyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	OnReadFriendsListComplete(FOnReadFriendsListComplete::CreateUObject(this, &UGetSteamFriendsCallProxyBase::OnGetSteamFriendsComplete))
{
}

UGetSteamFriendsCallProxyBase* UGetSteamFriendsCallProxyBase::GetSteamFriends(UObject* InWorldContextObject,
	APlayerController* InPlayerController)
{
	/* Create the proxy that will be reflected in blueprints as a custom node. */
	UGetSteamFriendsCallProxyBase* Proxy = NewObject<UGetSteamFriendsCallProxyBase>();
	Proxy->WorldContextObject = InWorldContextObject;
	Proxy->PlayerControllerWeakPtr = InPlayerController;

	return Proxy;
}

void UGetSteamFriendsCallProxyBase::Activate()
{
	Super::Activate();

	/* Ensure our player controller is valid. */
	if (PlayerControllerWeakPtr.IsValid())
	{
		/* Retrieve the friends interface from the online subsystem. */
		IOnlineFriendsPtr FriendInterface = Online::GetFriendsInterface();

		/* Ensure that the friends interface was successfully retrieved. */
		if (FriendInterface.IsValid())
		{
			/* Get the local player, whose friends list we are attempting to retrieve. */
			const ULocalPlayer* LocalPlayer = PlayerControllerWeakPtr->GetLocalPlayer();

			/* Query the online friends interface to retrieve the local player's friends list. Triggers the
			 * OnReadFriendsListComplete delegate when completed to call OnGetSteamFriendsComplete. */
			FriendInterface->ReadFriendsList(LocalPlayer->GetControllerId(), EFriendsLists::ToString(EFriendsLists::Default), OnReadFriendsListComplete);

			return;
		}
	}

	/* If anything fails, return that the call failed and an empty friend array. */
	TArray<FSteamFriend> SteamFriends;
	OnFailure.Broadcast(SteamFriends);
}

void UGetSteamFriendsCallProxyBase::OnGetSteamFriendsComplete(int32 LocalUserNum, bool bWasSuccessful,
	const FString& ListName, const FString& ErrorStr)
{
	/* The array of friends to return. */
	TArray<FSteamFriend> SteamFriends;

	/* Get the friend list information if the friends list was successfully retrieved. */
	if (bWasSuccessful)
	{
		/* Get the online friends interface. */
		IOnlineFriendsPtr FriendsInterface = Online::GetFriendsInterface();

		if (FriendsInterface.IsValid())
		{
			/* Get an array of every friend in the friends list. */
			TArray<TSharedRef<FOnlineFriend>> FriendList;
			FriendsInterface->GetFriendsList(LocalUserNum, ListName, OUT FriendList);

			/* Retrieve the information of every friend in the friends list. */
			for (TSharedRef<FOnlineFriend> Friend : FriendList)
			{
				UE_LOG(LogTemp, Error, TEXT("%s's ID: %s"), *Friend->GetDisplayName(), *Friend->GetUserId()->ToString());
				
				/* The default FOnlineFriend class isn't reflected in blueprints, so we need to save its information
				 * into our custom FSteamFriend structure. */
				FSteamFriend SteamFriend(Friend);

				/* Add the new Steam friend to our returned array. */
				SteamFriends.Add(SteamFriend);
			}

			/* Call the OnSuccess delegate when the friends list was successfully retrieved. */
			OnSuccess.Broadcast(SteamFriends);
		}
	}

	/* Call the OnFailure delegate when the friends list failed to be retrieved. */
	OnFailure.Broadcast(SteamFriends);
}
