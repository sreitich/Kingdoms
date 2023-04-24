// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_HUD.h"

#include "Interfaces/MenuTransitionInterface.h"
#include "UserDefinedData/Menu_UserDefinedData.h"
#include "UserInterface/MainMenu/MM_ArmyMenuWidget.h"
#include "UserInterface/MainMenu/MM_CollectionMenuWidget.h"
#include "UserInterface/MainMenu/MM_CollectionInfoPopUpWidget.h"
#include "UserInterface/MainMenu/MM_LobbyMenuWidget.h"
#include "UserInterface/MainMenu/MM_MainMenuWidget.h"
#include "UserInterface/MainMenu/MM_MapSelectionWidget.h"
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
		case E_CollectionMenu:
			CreateCollectionMenu(false);
			break;
		case E_LobbyMenu:
			CreateLobbyMenuWidget(false);
			break;
		case E_MapSelection:
			CreateMapSelectionWidget(false);
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
			if (IsValid(MainMenuWidget))
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
		if (IsValid(MainMenuWidget))
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
			if (IsValid(PlayMenuWidget))
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
		if (IsValid(PlayMenuWidget))
		{
			PlayMenuWidget->RemoveFromParent();
			PlayMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateLobbyMenuWidget(bool bDestroy)
{
	/* Create a lobby menu widget if requested. */
	if (!bDestroy)
	{
		if (LobbyMenuWidgetClass)
		{
			/* Create the widget. */
			LobbyMenuWidget = CreateWidget<UMM_LobbyMenuWidget>(GetOwningPlayerController(), LobbyMenuWidgetClass, FName("Lobby Menu Widget"));

			/* Add the lobby menu widget to the player's viewport if it was successfully created. */
			if (IsValid(LobbyMenuWidget))
			{
				LobbyMenuWidget->AddToViewport(0);
				CurrentMenu = LobbyMenuWidget;
			}
			/* If there was a problem creating the widget, return to the play menu. */
			else
			{
				QueueMenuChange(E_PlayMenu);
			}
		}
	}
	/* Destroy the lobby menu widget if requested. */
	else
	{
		if (IsValid(LobbyMenuWidget))
		{
			LobbyMenuWidget->RemoveFromParent();
			LobbyMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateMapSelectionWidget(bool bDestroy)
{
	/* Create a map selection menu widget if requested. */
	if (!bDestroy)
	{
		if (MapSelectionWidgetClass)
		{
			/* Create the widget. */
			MapSelectionWidget = CreateWidget<UMM_MapSelectionWidget>(GetOwningPlayerController(), MapSelectionWidgetClass, FName("Map Selection Menu Widget"));

			/* Add the map selection menu widget to the player's viewport if it was successfully created. */
			if (IsValid(MapSelectionWidget))
			{
				MapSelectionWidget->AddToViewport(0);
				CurrentMenu = MapSelectionWidget;
			}
			/* If there was a problem creating the widget, return to the lobby menu. */
			else
			{
				QueueMenuChange(E_LobbyMenu);
			}
		}
	}
	/* Destroy the map selection menu widget if requested. */
	else
	{
		if (IsValid(MapSelectionWidget))
		{
			MapSelectionWidget->RemoveFromParent();
			MapSelectionWidget = nullptr;
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
			if (IsValid(ArmyMenuWidget))
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
		if (IsValid(ArmyMenuWidget))
		{
			ArmyMenuWidget->RemoveFromParent();
			ArmyMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateCollectionMenu(bool bDestroy)
{
	/* Create a collection menu widget if requested. */
	if (!bDestroy)
	{
		if (CollectionMenuWidgetClass)
		{
			/* Create the widget. */
			CollectionMenuWidget = CreateWidget<UMM_CollectionMenuWidget>(GetOwningPlayerController(), CollectionMenuWidgetClass, FName("Collection Menu Widget"));

			/* Add the collection menu widget to the player's viewport if it was successfully created. */
			if (IsValid(CollectionMenuWidget))
			{
				CollectionMenuWidget->AddToViewport(0);
				CurrentMenu = CollectionMenuWidget;
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_MainMenu);
			}
		}
	}
	/* Destroy the collection menu widget if requested. */
	else
	{
		if (IsValid(CollectionMenuWidget))
		{
			CollectionMenuWidget->RemoveFromParent();
			CollectionMenuWidget = nullptr;
		}
	}
}

void AMM_HUD::CreateCollectionPieceInfoPopUp(bool bDestroy, FPieceDataStruct NewPieceData)
{
	/* Create a collection piece info widget if requested. */
	if (!bDestroy)
	{
		if (CollectionInfoPopUpWidgetClass)
		{
			/* Create the widget. */
			CollectionInfoPopUpWidget = CreateWidget<UMM_CollectionInfoPopUpWidget>(GetOwningPlayerController(), CollectionInfoPopUpWidgetClass, FName("Collection Piece Info Pop Up Widget"));

			/* Update the collection piece info widget's displayed information and add it to the player's viewport if it was successfully created. */
			if (IsValid(CollectionInfoPopUpWidget))
			{
				CollectionInfoPopUpWidget->UpdateAndActivatePopUp(NewPieceData);
				CollectionInfoPopUpWidget->AddToViewport(1);
			}
			/* If there was a problem creating the widget, return to the main menu. */
			else
			{
				QueueMenuChange(E_MainMenu);
			}
		}
	}
	/* Destroy the collection piece info widget if requested. */
	else
	{
		if (IsValid(CollectionInfoPopUpWidget))
		{
			CollectionInfoPopUpWidget->RemoveFromParent();
			CollectionInfoPopUpWidget = nullptr;
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
			if (IsValid(StoreWidget))
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
		if (IsValid(StoreWidget))
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
			if (IsValid(OptionsWidget))
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
		if (IsValid(MatchmakingStartPopUpWidget))
		{
			MatchmakingStartPopUpWidget->AddToViewport(1);
		}
	}
}

void AMM_HUD::CreateQueueTimerWidget(bool bDestroy)
{
	/* Create a queue timer widget if requested. */
	if (!bDestroy)
	{
		/* Ensure that the widget class to spawn is set. */
		if (QueueTimerClass)
		{
			/* Create the widget. */
			QueueTimerWidget = CreateWidget<UMM_QueueTimer>(GetOwningPlayerController(), QueueTimerClass, FName("Queue Timer Widget"));

			/* Add the queue timer widget to the player's viewport if it was successfully created. */
			if (IsValid(QueueTimerWidget))
			{
				QueueTimerWidget->AddToViewport(6);
			}
		}
	}
	/* Safely destroy the queue timer widget if requested. */
	else
	{
		if (IsValid(QueueTimerWidget))
		{
			QueueTimerWidget->RemoveFromParent();
			QueueTimerWidget = nullptr;
		}
	}
}

void AMM_HUD::HUD_FoundOpponent()
{
	/* Call the queue timer widget's FoundOpponent function, which creates a new pop-up and disables the "cancel"
	 * button. */
	if (IsValid(QueueTimerWidget))
	{
		QueueTimerWidget->FoundOpponent();
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
