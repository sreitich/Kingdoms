// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Menu_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MM_PlayerController.generated.h"

class AMM_LobbyBeaconClient;
class UMM_MatchmakingStartPopUp;
class UMM_QueueTimer;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_PlayerController : public APlayerController
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_PlayerController();


/* Public variables. */
public:

	/* Whether or not this player is currently hosting a custom lobby, if they are in one. */
	UPROPERTY(BlueprintReadWrite, Category="Custom Lobby")
	bool bIsCustomLobbyHost = false;

	/* A reference to this player's client beacon, if they are connected to a custom lobby as a client. */
	UPROPERTY(BlueprintReadWrite, Category="Custom Lobby")
	AMM_LobbyBeaconClient* CustomLobbyClientBeacon;

	/* Used to track which army preset the player currently has selected. This is stored here to save this data when
	 * switching between menus. */
	UPROPERTY(BlueprintReadOnly)
	uint8 SelectedArmyPreset;


/* Protected functions. */
protected:

	/* Called when spawned. */
	virtual void BeginPlay() override;


/* Protected widgets. */
protected:

	/* A reference to a currently displayed matchmaking start pop-up widget. */
	UPROPERTY()
	UMM_MatchmakingStartPopUp* MatchmakingStartPopUpWidget;

	/* A reference to a currently displayed queue timer widget. */
	UPROPERTY()
	UMM_QueueTimer* QueueTimerWidget;


/* Protected widget class references. */
protected:

	/* The class to spawn when creating a matchmaking start pop-up widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="User Interface");
	TSubclassOf<UUserWidget> MatchmakingStartPopUpClass;

	/* The class to spawn when creating a queue timer widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="User Interface");
	TSubclassOf<UUserWidget> QueueTimerClass;
};
