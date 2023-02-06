// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_LobbyMenuWidget.h"

#include "UserInterface/MainMenu/MM_HUD.h"

#include "Components/Button.h"
#include "Framework/KingdomsGameInstance.h"

void UMM_LobbyMenuWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_LobbyMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the "invite friends" button to open a list of friends to invite. */
	InviteFriendsButton->OnClicked.AddDynamic(this, &UMM_LobbyMenuWidget::OnInviteFriendsClicked);
	/* Bind the "back" button to deactivate this widget.*/
	BackButton->OnClicked.AddDynamic(this, &UMM_LobbyMenuWidget::OnBackClicked);

	/* Play the activation animation to reveal this widget. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Bind the deactivation animation to return to the play menu after it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_LobbyMenuWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);
}

void UMM_LobbyMenuWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreateLobbyMenuWidget(true);
}

void UMM_LobbyMenuWidget::OnInviteFriendsClicked()
{
	/* Display the subsystem's external friends UI via the game instance. */
	if (UKingdomsGameInstance* GameInstancePtr = GetGameInstance<UKingdomsGameInstance>())
	{
		GameInstancePtr->ShowFriendsUI();
	}
}

void UMM_LobbyMenuWidget::OnBackClicked()
{
	/* To navigate back to the play menu, queue the menu, which will deactivate this menu and transition to the new
	 * one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_PlayMenu);
}
