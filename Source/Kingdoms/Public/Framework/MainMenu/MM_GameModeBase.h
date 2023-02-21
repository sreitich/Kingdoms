// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MM_GameModeBase.generated.h"

class AMM_LobbyBeaconHostObject;
class AOnlineBeaconHost;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_GameModeBase();

	/* Called when a player enters the map. Starts the game if there are enough players connected. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/* Creates and registers a beacon host object to host a lobby. */
	UFUNCTION()
	bool CreateHostBeacon();

	/* Getter for beacon host object. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AMM_LobbyBeaconHostObject* GetBeaconHostObject() const { return BeaconHostObject; }


/* Protected variables. */
protected:

	/* Persistent pointer to the beacon host, if we are currently hosting a custom lobby. */
	UPROPERTY()
	AOnlineBeaconHost* BeaconHost = nullptr;

	/* Persistent pointer to the beacon host object, if we are currently hosting a custom lobby. */
	UPROPERTY()
	AMM_LobbyBeaconHostObject* BeaconHostObject = nullptr;

};
