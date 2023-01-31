// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_PlayerController.h"

#include "UserInterface/MainMenu/MM_QueueTimer.h"

AMM_PlayerController::AMM_PlayerController()
{
	/* Enable mouse controls. */
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AMM_PlayerController::CreateQueueTimerWidget()
{
	/* Create and display a new queue timer widget if there is not one already exist.*/
	if (QueueTimerClass && !IsValid(QueueTimerWidget))
	{
		QueueTimerWidget = CreateWidget<UMM_QueueTimer>(this, QueueTimerClass, FName("Queue Timer Widget"));

		if (IsValid(QueueTimerWidget))
		{
			QueueTimerWidget->AddToViewport(1);
		}
	}
}

void AMM_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	/* Only allow the player to make inputs in the user interface. */
	SetInputMode(FInputModeUIOnly());
}
