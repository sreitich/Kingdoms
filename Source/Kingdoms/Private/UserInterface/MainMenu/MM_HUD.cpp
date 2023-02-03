// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_HUD.h"

#include "Interfaces/MenuTransitionInterface.h"
#include "UserDefinedData/Menu_UserDefinedData.h"
#include "UserInterface/MainMenu/MM_ArmyMenuWidget.h"
#include "UserInterface/MainMenu/MM_MainMenuWidget.h"
#include "UserInterface/MainMenu/MM_MatchmakingStartPopUp.h"
#include "UserInterface/MainMenu/MM_OptionsWidget.h"
#include "UserInterface/MainMenu/MM_PlayMenuWidget.h"
#include "UserInterface/MainMenu/MM_QueueTimer.h"
#include "UserInterface/MainMenu/MM_StoreWidget.h"

#include "Blueprint/UserWidget.h"

AMM_HUD::AMM_HUD()
{
}

void AMM_HUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMM_HUD::QueueMenuChange(EMenuScreen NewMenu)
{
	/* Queue the next menu. */
	QueuedMenu = NewMenu;

	/* Deactivate the current menu, which will call ChangeMenus() when it finishes deactivating. */
	if (CurrentMenu && CurrentMenu->GetClass()->ImplementsInterface(UMenuTransitionInterface::StaticClass()))
	{
		Cast<IMenuTransitionInterface>(CurrentMenu)->DeactivateWidget();
	}
}

void AMM_HUD::ChangeMenus()
{
	/* Create the next queued menu. */
	switch (QueuedMenu)
	{
		case E_ArmyMenu:
			CreateArmyMenuWidget(false);
			break;
		case E_MainMenu:
			CreateMainMenuWidget(false);
			break;
		case E_OptionsScreen:
			CreateOptionsWidget(false);
			break;
		case E_PlayMenu:
			CreatePlayMenuWidget(false);
			break;
		case E_StoreScreen:
			CreateStoreWidget(false);
			break;
		/* Return to the main menu by default if there's is a problem. */
		default:
			CreateMainMenuWidget(false);
	}
}

void AMM_HUD::CreateMainMenuWidget(bool bDestroy)
{
	/* Create a main menu widget if requested. */
	if (!bDestroy)
	{
		if (MainMenuWidgetClass)
		{
			/* Create the widget. */
			MainMenuWidget = CreateWidget<UMM_MainMenuWidget>(GetOwningPlayerController(), MainMenuWidgetClass, FName("Main Menu Widget"));

			/* Add the main menu widget to the player's viewport if it was successfully created. */
			if (MainMenuWidget)
			{
				MainMenuWidget->AddToViewport(0);
				CurrentMenu = MainMenuWidget;
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_MainMenu);
			}
		}
	}
	/* Destroy the main menu widget if requested. */
	else
	{
		if (MainMenuWidget)
		{
			MainMenuWidget->RemoveFromParent();
			MainMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreatePlayMenuWidget(bool bDestroy)
{
	/* Create a play menu widget if requested. */
	if (!bDestroy)
	{
		if (PlayMenuWidgetClass)
		{
			/* Create the widget. */
			PlayMenuWidget = CreateWidget<UMM_PlayMenuWidget>(GetOwningPlayerController(), PlayMenuWidgetClass, FName("Play Menu Widget"));

			/* Add the play menu widget to the player's viewport if it was successfully created. */
			if (PlayMenuWidget)
			{
				PlayMenuWidget->AddToViewport(0);
				CurrentMenu = PlayMenuWidget;
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_MainMenu);
			}
		}
	}
	/* Destroy the play menu widget if requested. */
	else
	{
		if (PlayMenuWidget)
		{
			PlayMenuWidget->RemoveFromParent();
			PlayMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateArmyMenuWidget(bool bDestroy)
{
	/* Create an army menu widget if requested. */
	if (!bDestroy)
	{
		if (ArmyMenuWidgetClass)
		{
			/* Create the widget. */
			ArmyMenuWidget = CreateWidget<UMM_ArmyMenuWidget>(GetOwningPlayerController(), ArmyMenuWidgetClass, FName("Army Menu Widget"));

			/* Add the army menu widget to the player's viewport if it was successfully created. */
			if (ArmyMenuWidget)
			{
				ArmyMenuWidget->AddToViewport(0);
				CurrentMenu = ArmyMenuWidget;
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_MainMenu);
			}
		}
	}
	/* Destroy the army menu widget if requested. */
	else
	{
		if (ArmyMenuWidget)
		{
			ArmyMenuWidget->RemoveFromParent();
			ArmyMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateStoreWidget(bool bDestroy)
{
	/* Create a store widget if requested. */
	if (!bDestroy)
	{
		if (StoreWidgetClass)
		{
			/* Create the widget. */
			StoreWidget = CreateWidget<UMM_StoreWidget>(GetOwningPlayerController(), StoreWidgetClass, FName("Store Widget"));

			/* Add the store widget to the player's viewport if it was successfully created. */
			if (StoreWidget)
			{
				StoreWidget->AddToViewport(0);
				CurrentMenu = StoreWidget;
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_MainMenu);
			}
		}
	}
	/* Destroy the store widget if requested. */
	else
	{
		if (StoreWidget)
		{
			StoreWidget->RemoveFromParent();
			StoreWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateOptionsWidget(bool bDestroy)
{
	/* Create an options widget if requested. */
	if (!bDestroy)
	{
		if (OptionsWidgetClass)
		{
			/* Create the widget. */
			OptionsWidget = CreateWidget<UMM_OptionsWidget>(GetOwningPlayerController(), OptionsWidgetClass, FName("Options Widget"));

			/* Add the options widget to the player's viewport if it was successfully created. */
			if (OptionsWidget)
			{
				OptionsWidget->AddToViewport(0);
				CurrentMenu = OptionsWidget;
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_OptionsScreen);
			}
		}
	}
	/* Destroy the options widget if requested. */
	else
	{
		if (OptionsWidget)
		{
			OptionsWidget->RemoveFromParent();
			OptionsWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateMatchmakingStartPopUpWidget()
{
	/* Ensure that the widget class to spawn is set. */
	if (MatchmakingStartPopUpClass)
	{
		/* Create the widget. */
		MatchmakingStartPopUpWidget = CreateWidget<UMM_MatchmakingStartPopUp>(GetOwningPlayerController(), MatchmakingStartPopUpClass, FName("Matchmaking Start Pop-Up Widget"));

		/* Add the matchmaking start pop-up widget to the player's viewport if it was successfully created. */
		if (MatchmakingStartPopUpWidget)
		{
			MatchmakingStartPopUpWidget->AddToViewport(1);
		}
	}
}

void AMM_HUD::CreateQueueTimerWidget()
{
	/* Ensure that the widget class to spawn is set. */
	if (QueueTimerClass)
	{
		/* Create the widget. */
		QueueTimerWidget = CreateWidget<UMM_QueueTimer>(GetOwningPlayerController(), QueueTimerClass, FName("Queue Timer Widget"));

		/* Add the queue timer widget to the player's viewport if it was successfully created. */
		if (QueueTimerWidget)
		{
			QueueTimerWidget->AddToViewport(6);
		}
	}
}

void AMM_HUD::BeginPlay()
{
	Super::BeginPlay();

	/* Set the current menu to be the main menu. */
	// CurrentMenu = MainMenuWidget;

	/* Create a main menu widget when the game starts. */
	// CreateMainMenuWidget(false);
}
