// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapData.generated.h"

/* Types of maps (i.e. game-modes). */
UENUM(BlueprintType)
enum EMapType
{
	E_Standard		UMETA(DisplayName="Standard"),
	E_Resized		UMETA(DisplayName="Resized"),
	E_Disruption	UMETA(DisplayName="Disruption"),
	E_Timer			UMETA(DisplayName="Timer"),
	E_Chaos			UMETA(DisplayName="Chaos")
};

/**
 * 
 */
UCLASS(BlueprintType)
class KINGDOMS_API UMapData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FName DisplayedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FText GameDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	TEnumAsByte<EMapType> MapType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Images")
	UTexture2D* CustomGamePreviewImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Images")
	UTexture2D* CustomGameSelectionMenuImage = nullptr;

};
