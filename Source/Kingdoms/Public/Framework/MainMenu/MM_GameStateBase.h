// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Menu_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MM_GameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Returns the local player's current settings for their custom game if they are hosting a custom lobby. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FCustomGameSettings GetCustomGameSettings() { return CustomGameSettings; }

	/* Updates the local player's custom game settings if they are hosting a custom lobby. */
	UFUNCTION(BlueprintCallable)
	void SetCustomGameSettings(FCustomGameSettings NewCustomGameSettings);


/* Protected variables. */
protected:

	/* The settings for a custom game, if the local player is currently hosting a custom lobby. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Game Settings")
	FCustomGameSettings CustomGameSettings;
};
