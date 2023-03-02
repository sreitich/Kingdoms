// Copyright Change Studios, LLC 2023.

#pragma once

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
};
