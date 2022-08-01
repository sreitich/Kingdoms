// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KingdomsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UKingdomsGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* Public functions. */
public:

	virtual void Init() override;


/* Public variables. */
public:

	/* References to save game objects to use when saving and loading. */
	UPROPERTY()
	class UUnlockedPieces_SaveGame* UnlockedPieces_SaveGame;

	UPROPERTY()
	class UArmyPresets_SaveGame* ArmyPresets_SaveGame;

	
};
