// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_MainMenuWidget.h"

#include "UserInterface/MainMenu/MM_HUD.h"

#include "Components/Button.h"

void UMM_MainMenuWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_MainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the navigation buttons to navigate to their respect menus. */
	// PlayNavigationButton->OnClicked.AddDynamic(this, &UMM_MainMenuWidget::OnPlayButton);
	// ArmyNavigationButton->OnClicked.AddDynamic(this, &UMM_MainMenuWidget::OnArmyButton);
	// StoreNavigationButton->OnClicked.AddDynamic(this, &UMM_MainMenuWidget::OnStoreButton);
	// OptionsNavigationButton->OnClicked.AddDynamic(this, &UMM_MainMenuWidget::OnOptionsButton);

	/* Play the activation animation to reveal this widget. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Bind the deactivation animation to destroy this widget when it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_MainMenuWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);
}

void UMM_MainMenuWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreateMainMenuWidget(true);
}

void UMM_MainMenuWidget::NavigateToMenu(EMenuScreen NewMenu)
{
	/* To navigate to a new menu, queue the menu, which will deactivate this menu and transition to the new one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(NewMenu);
}
