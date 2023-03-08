// Copyright Change Studios, LLC 2023.

#pragma once

#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_LobbyFriendList.generated.h"

class UVerticalBox;

UCLASS()
class KINGDOMS_API UMM_LobbyFriendList : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called when this widget is created. */
	virtual void NativeConstruct() override;

	/* Creates and displays a friends list entry for every given friend. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Steam|Friends")
	void PopulateFriendsList(const TArray<FSteamFriend>& FriendsList);
};
