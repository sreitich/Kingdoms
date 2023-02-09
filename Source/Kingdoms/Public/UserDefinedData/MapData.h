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

/* All metadata pertaining to playable in-game maps. */
UCLASS(BlueprintType)
class KINGDOMS_API UMapData : public UDataAsset
{
	GENERATED_BODY()

public:

	/* The map's displayed name in-game. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FName DisplayedName = FName();

	/* The map's description when viewing it in the custom lobby map selection menu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FText GameDescription = FText();

	/* The address of the map's ULevel asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	FString Path = FString();

	/* Which type of map (i.e. game mode) this is. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metadata")
	TEnumAsByte<EMapType> MapType = E_Standard;

	/* The image displayed in a custom lobby menu, previewing the lobby's selected map. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Images")
	UTexture2D* CustomGameLobbyPreviewImage = nullptr;

	/* The image displayed in a custom lobby map selection menu, previewing a map that can be selected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Images")
	UTexture2D* CustomGameSelectionMenuPreviewImage = nullptr;

};
