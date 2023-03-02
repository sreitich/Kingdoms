// Copyright Change Studios, LLC 2023.

#pragma once

#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_LobbyFriendListEntry.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_LobbyFriendListEntry : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called when this widget is created. */
	virtual void NativeConstruct() override;


/* Public variables. */
public:

	/* The Steam friend that this widget represents. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Category="Steam|Friends")
	FSteamFriend SteamFriend;
};
