// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_PlayerController.h"

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
}
