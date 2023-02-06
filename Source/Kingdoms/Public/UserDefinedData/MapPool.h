// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/MapData.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapPool.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMapPool : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FName DisplayedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maps")
	TArray<UMapData*> Maps;
	
};
