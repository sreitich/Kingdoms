// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UserSettings_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UUserSettings_SaveGame : public USaveGame
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	UUserSettings_SaveGame();


/* Public variables. */
public:

	/* Info used when storing and loading this save. */
	UPROPERTY(VisibleAnywhere, Category="Save File Info")
	FString SaveSlotName;
 
	UPROPERTY(VisibleAnywhere, Category="Save File Info")
	int UserIndex;
};
