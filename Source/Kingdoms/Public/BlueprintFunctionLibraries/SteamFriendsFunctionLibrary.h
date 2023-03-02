// Copyright Change Studios, LLC 2023.

#pragma once

#include "Interfaces/OnlinePresenceInterface.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamFriendsFunctionLibrary.generated.h"

class UTexture2D;

/**
 *	Enum used to store the current status of Steam friends.
 */
UENUM(Blueprintable)
enum class ESteamFriendStatus : uint8
{
	E_PlayingSameGame = 0	UMETA(DisplayName="Playing Same Game"),
	E_InGame = 1			UMETA(DisplayName="In-Game"),
	E_Online = 2			UMETA(DisplayName="Online"),
	E_Offline = 3			UMETA(DisplayName="Offline")
};

/**
 *	Struct defining a SteamFriend, which can be accessed in Blueprints.
 */
USTRUCT(BlueprintType)
struct FSteamFriend
{
	GENERATED_USTRUCT_BODY()

public:

	/* The player's display name. */
	UPROPERTY(BlueprintReadOnly, Category="Steam | Friends")
	FString DisplayName;

	/* The player's real name, if it's visible to the local player. */
	UPROPERTY(BlueprintReadOnly, Category="Steam | Friends")
	FString RealName;

	/* The player's current status. */
	UPROPERTY(BlueprintReadOnly, Category="Steam | Friends")
	ESteamFriendStatus Status;

	/* The player's current status as a string. */
	UPROPERTY(BlueprintReadOnly, Category="Steam | Friends")
	FString StatusString;

	/* Whether the player's current game is joinable. */
	UPROPERTY(BlueprintReadOnly, Category="Steam | Friends")
	bool bIsJoinable;

	/* Whether the player has voice support. */
	UPROPERTY(BlueprintReadOnly, Category="Steam | Friends")
	bool bHasVoiceSupport;

	/* The enumerated presence state of the player (offline, away, etc.). */
	EOnlinePresenceState::Type PresenceState;

	/* The player's unique net ID. */
	TSharedPtr<const FUniqueNetId> UniqueNetID;

	FSteamFriend() = default;

	/* Convert the built-in Steam friend structure into our custom Steam friend structure. */
	FSteamFriend(TSharedPtr<FOnlineFriend> Friend)
	{
		FOnlineUserPresence Presence = Friend->GetPresence();

		DisplayName = Friend->GetDisplayName();
		RealName = Friend->GetRealName();
		StatusString = Presence.Status.StatusStr;
		bHasVoiceSupport = Presence.bHasVoiceSupport;
		PresenceState = (EOnlinePresenceState::Type) (int32) Presence.Status.State;
		UniqueNetID = Friend->GetUserId();

		if (Presence.bIsPlayingThisGame)
			Status = ESteamFriendStatus::E_PlayingSameGame;
		else if (Presence.bIsPlaying)
			Status = ESteamFriendStatus::E_InGame;
		else if (Presence.bIsOnline)
			Status = ESteamFriendStatus::E_Online;
		else
			Status = ESteamFriendStatus::E_Offline;
	}

	/** @return whether the given friend should be listed below this friend. */
	bool operator<(const FSteamFriend& Other) const
	{
		/* Online friends are listed before offline friends. */
		if (Status < Other.Status)
			return true;
		if (Status > Other.Status)
			return false;

		/* Friends with the same status are listed in alphabetical order. */
		return DisplayName.ToLower() < Other.DisplayName.ToLower() ? true : false;
	}

	/* @return Whether the given Steam friend has the same unique net ID as this friend. */
	bool operator==(const FSteamFriend& Other) const
	{
		return UniqueNetID == Other.UniqueNetID;
	}
};

/**
 *	Enum used for the Execs of the Static Functions.
 */
UENUM()
enum class ESteamFriendAsyncResultSwitch : uint8
{
	OnSuccess,
	AsyncLoading,
	OnFailure
};

/**
 *	Enum use for requesting a specific Avatar size.
 */
UENUM(Blueprintable)
enum class ESteamAvatarSize : uint8
{
	E_SteamSmall = 0	UMETA(DisplayName="Small Steam Avatar"),
	E_SteamMedium= 1	UMETA(DisplayName="Medium Steam Avatar"),
	E_SteamLarge= 2		UMETA(DisplayName="Large Steam Avatar")
};

/**
 *	Library for useful SteamFriends Functions.
 */
UCLASS()
class KINGDOMS_API USteamFriendsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

/* Blueprint-accessible functions. */
public:

	/**
	*	Gets the Avatar of the specified Steam Friend.
	*
	*	@param		SteamFriend		Steam Friend we want the Avatar from
	*	@param		Result			Result Enum used for Execs
	*	@param		AvatarSize		Size of the Avatar we want (default: Medium)
	*/
	UFUNCTION(BlueprintCallable, Category = "Steam|Friends", meta = (ExpandEnumAsExecs = "Result"))
		static UTexture2D* GetSteamFriendAvatar(const FSteamFriend SteamFriend, ESteamFriendAsyncResultSwitch& Result, ESteamAvatarSize AvatarSize = ESteamAvatarSize::E_SteamMedium);

	/**
	*	Gets the Game of the specified Steam Friend.
	*
	*	@param		SteamFriend		Steam Friend we want the Game from
	*	@param		Result			Result Enum used for Execs
	*	@param		GameName		Name of the Game (if found)
	*/
	UFUNCTION(BlueprintCallable, Category = "Steam|Friends", meta = (ExpandEnumAsExecs = "Result"))
		static void GetSteamFriendGame(const FSteamFriend SteamFriend, ESteamFriendAsyncResultSwitch& Result, FString& GameName);


	/**
	 *	Sorts the given array of Steam friends. Lists players playing the same game, then players playing any game,
	 *	then online players, then offline players. Each subsection is sorted alphabetically.
	 *
	 *	@param		SteamFriends	Array to sort
	 *	@return						The sorted array
	 */
	UFUNCTION(BlueprintCallable, Category = "Steam|Friends")
		static TArray<FSteamFriend> SortSteamFriends(TArray<FSteamFriend> SteamFriends);

	UFUNCTION(BlueprintPure, meta=(DisplayName = "== (Steam Friend)", CompactNodeTitle = "==", Keywords = "equal equals equality = =="), Category = "Math|Steam")
		static bool TestEquality_SteamFriend(FSteamFriend A, FSteamFriend B) { return A == B; }
};
