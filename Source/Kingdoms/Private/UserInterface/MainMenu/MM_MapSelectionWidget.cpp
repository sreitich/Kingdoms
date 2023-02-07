// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_MapSelectionWidget.h"

#include "UserInterface/MainMenu/MM_HUD.h"

void UMM_MapSelectionWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_MapSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Play the activation animation to reveal this widget. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Bind the deactivation animation to return to the play menu after it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_MapSelectionWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);
}

void UMM_MapSelectionWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreateMapSelectionWidget(true);
}

void UMM_MapSelectionWidget::ReturnToLobbyMenu()
{
	/* To navigate back to the lobby menu, queue the menu, which will deactivate this menu and transition to the new
	 * one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_LobbyMenu);
}
