// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LobbyDataKeys.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class KINGDOMS_API ULobbyDataKeys : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FString Map_Key = "Map";

};
