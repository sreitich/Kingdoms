// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_PlayMenuWidget.h"

#include "Framework/KingdomsGameInstance.h"
#include "UserInterface/MainMenu/MM_HUD.h"

#include "Components/Button.h"
#include "Framework/MainMenu/MM_GameModeBase.h"
#include "Framework/MainMenu/MM_PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMM_PlayMenuWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_PlayMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the deactivation animation to destroy this widget when it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_PlayMenuWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);

	/* Grab a persistent pointer to the game instance. */
	GameInstancePtr = Cast<UKingdomsGameInstance>(UGameplayStatics::GetGameInstance(GetOwningPlayer()));

	/* If the player is not in a session, play an animation to reveal this widget. This is done to help the illusion
	 * of remaining in the same menu when creating a new session. */
	if (IsValid(GameInstancePtr))
	{
		/* Check if the player is in a session. */
		bool bIsInSession, bIsHost;
		GameInstancePtr->GetCurrentSessionInfo(bIsInSession, bIsHost);
		if (!bIsInSession)
		{
			/* Play the activation animation to reveal this widget. */
			PlayAnimationForward(OnActivatedAnim, 1.0f, false);
		}
	}
}

void UMM_PlayMenuWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreatePlayMenuWidget(true);
}

void UMM_PlayMenuWidget::TryQuickplay()
{
	/* Join an available session. If no sessions are available, create a new one. */
	if (IsValid(GameInstancePtr))
	{
		if (AMM_PlayerController* PlayerControllerPtr = GetOwningPlayer<AMM_PlayerController>())
		{
			/* Disable player input and all buttons while searching for a session. */
			PlayerControllerPtr->DisableInput(PlayerControllerPtr);
			QuickPlayButton->SetIsEnabled(false);
			CustomGameButton->SetIsEnabled(false);
			BackButton->SetIsEnabled(false);

			/* Attempt to join a session. */
			GameInstancePtr->JoinServer();

			/* Create a matchmaking start pop-up widget. */
			AMM_HUD* HUDPtr = PlayerControllerPtr->GetHUD<AMM_HUD>();
			HUDPtr->CreateMatchmakingStartPopUpWidget();
		}
	}
}

void UMM_PlayMenuWidget::CreateCustomGame()
{
	/* Create a host beacon to host the new lobby. */
	if (AMM_GameModeBase* GameModePtr = Cast<AMM_GameModeBase>(UGameplayStatics::GetGameMode(GetOwningPlayer())))
	{
		GameModePtr->CreateHostBeacon();
	}

	/* To navigate to the lobby menu, queue the menu, which will deactivate this menu and transition to the new one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_LobbyMenu);
}

void UMM_PlayMenuWidget::ReturnToMainMenu()
{
	/* To navigate back to the main menu, queue the menu, which will deactivate this menu and transition to the new
	 * one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_MainMenu);
}

