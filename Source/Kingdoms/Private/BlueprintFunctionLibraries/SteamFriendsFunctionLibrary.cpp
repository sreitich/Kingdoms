// Copyright Change Studios, LLC 2023.


#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"

#include "steam/steam_api.h"


UTexture2D* USteamFriendsFunctionLibrary::GetSteamFriendAvatar(const FSteamFriend SteamFriend, ESteamFriendAsyncResultSwitch& Result, ESteamAvatarSize AvatarSize /*= ESteamAvatarSize::E_SteamMedium*/)
{
	if (!SteamFriend.UniqueNetID.IsValid())
	{
		Result = ESteamFriendAsyncResultSwitch::OnFailure;
		return nullptr;
	}

	// Size of the Texture
	uint32 Height = 0;
	uint32 Width = 0;

	if (SteamAPI_Init())
	{
		// Convert the ID to a uint64 pointer
		uint64 ID = *((uint64*)SteamFriend.UniqueNetID->GetBytes());

		// Set the Avatar Size Number
		int AvatarSizeNumber = 0;

		switch (AvatarSize)
		{
			case ESteamAvatarSize::E_SteamSmall:
				AvatarSizeNumber = SteamFriends()->GetSmallFriendAvatar(ID);
				break;
			case ESteamAvatarSize::E_SteamMedium:
				AvatarSizeNumber = SteamFriends()->GetMediumFriendAvatar(ID);
				break;
			case ESteamAvatarSize::E_SteamLarge:
				AvatarSizeNumber = SteamFriends()->GetLargeFriendAvatar(ID);
				break;
			default:
				break;
		}

		// Still Loading...
		if (AvatarSizeNumber == -1)
		{
			Result = ESteamFriendAsyncResultSwitch::AsyncLoading;
			return nullptr;
		}

		// Retrieve the actual Image Size, because we need that to reserve enough memory
		SteamUtils()->GetImageSize(AvatarSizeNumber, &Width, &Height);

		if (Height > 0 && Width > 0)
		{
			// Buffer for RGBA Data (4 * cause of R G B A)
			uint8* AvatarRGBA = new uint8[Height * Width * 4];

			SteamUtils()->GetImageRGBA(AvatarSizeNumber, AvatarRGBA, Height * Width * 4 * sizeof(char));

			// Create a Placeholder Texture to fill the Buffer in
			UTexture2D* AvatarTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

			// Lock the BulkData and MemCopy
			uint8* MipData = (uint8*)AvatarTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)AvatarRGBA, Height * Width * 4);
			AvatarTexture->PlatformData->Mips[0].BulkData.Unlock();
		
			// Clear up the memory!
			delete[] AvatarRGBA;

			// Update the Texture2D Settings
			AvatarTexture->PlatformData->SetNumSlices(1);
			AvatarTexture->NeverStream = true;
			AvatarTexture->UpdateResource();

			Result = ESteamFriendAsyncResultSwitch::OnSuccess;

			return AvatarTexture;
		}
	}

	Result = ESteamFriendAsyncResultSwitch::OnFailure;
	return nullptr;
}

void USteamFriendsFunctionLibrary::GetSteamFriendGame(const FSteamFriend SteamFriend, ESteamFriendAsyncResultSwitch& Result, FString& GameName)
{
	if (!SteamFriend.UniqueNetID.IsValid())
	{
		Result = ESteamFriendAsyncResultSwitch::OnFailure;
		return;
	}

	if (SteamAPI_Init())
	{
		// Convert the ID to a uint64 pointer
		uint64 ID = *((uint64*)SteamFriend.UniqueNetID->GetBytes());

		// Get the GameInfo from the Player via ID
		FriendGameInfo_t GameInfo;
		bool bIsInGame = SteamFriends()->GetFriendGamePlayed(ID, &GameInfo);

		// If InGame and the GameID is actually valid, try to get the AppName
		if (bIsInGame && GameInfo.m_gameID.IsValid())
		{
			char* NameBuffer = new char[1024];
			int NameLength = SteamAppList()->GetAppName(GameInfo.m_gameID.AppID(), NameBuffer, 1024);

			// If the app name was found, convert it into a String.
			if (NameLength != -1)
			{
				GameName = FString(UTF8_TO_TCHAR(NameBuffer));
			}

			Result = ESteamFriendAsyncResultSwitch::OnSuccess;
			return;
		}
	}

	Result = ESteamFriendAsyncResultSwitch::OnFailure;
}

TArray<FSteamFriend> USteamFriendsFunctionLibrary::SortSteamFriends(TArray<FSteamFriend> SteamFriends)
{
	/* The < operator is defined for FSteamFriend, so we can use the built-in TArray::Sort method. */
	SteamFriends.Sort();
	return SteamFriends;
}
