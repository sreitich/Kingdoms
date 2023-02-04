// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_PlayerController.h"

#include "Framework/KingdomsGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "UserInterface/MainMenu/MM_HUD.h"

AMM_PlayerController::AMM_PlayerController()
{
	/* Enable mouse controls. */
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AMM_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	/* Only allow the player to make inputs in the user interface. */
	SetInputMode(FInputModeUIOnly());

	/* If the player loads into the main menu as the host of a new session, then they have begun matchmaking. To make
	 * it look like they didn't change actually maps, we'll return them to the play menu they were just in before
	 * creating a new queue timer widget. */
	if (const UKingdomsGameInstance* GameInstancePtr = Cast<UKingdomsGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		/* Check if the player is in a session and the host of that session. */
		bool bIsInSession, bIsHost;
		GameInstancePtr->GetCurrentSessionInfo(bIsInSession, bIsHost);
		if (bIsInSession && bIsHost)
		{
			/* Switch back to the play menu and create a queue timer widget. */
			if (AMM_HUD* HUDPtr = GetHUD<AMM_HUD>())
			{
				HUDPtr->CreatePlayMenuWidget(false);
				HUDPtr->CreateQueueTimerWidget(false);
			}
		}
		/* If the player is entering the main menu from anywhere else, create the main menu widget. */
        else
        {
        	if (AMM_HUD* HUDPtr = GetHUD<AMM_HUD>())
        	{
        		HUDPtr->CreateMainMenuWidget(false);
        	}
        }
	}
}
