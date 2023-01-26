// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_PlayMenuWidget.h"

#include "Framework/KingdomsGameInstance.h"
#include "UserInterface/MainMenu/MM_HUD.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMM_PlayMenuWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_PlayMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the quick-play button to find/create and join then a session. */
	QuickPlayButton->OnClicked.AddDynamic(this, &UMM_PlayMenuWidget::OnQuickPlayClicked);
	/* Bind the invite button to open the Steam friend invite interface. */
	InviteFriendButton->OnClicked.AddDynamic(this, &UMM_PlayMenuWidget::OnInviteFriendClicked);
	/* Bind the "back" button to deactivate this widget. */
	BackButton->OnClicked.AddDynamic(this, &UMM_PlayMenuWidget::OnBackClicked);

	/* Play the activation animation to reveal this widget. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Bind the deactivation animation to destroy this widget when it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_PlayMenuWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);

	/* Grab a persistent pointer to the game instance. */
	GameInstancePtr = Cast<UKingdomsGameInstance>(UGameplayStatics::GetGameInstance(GetOwningPlayer()));
}

void UMM_PlayMenuWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreatePlayMenuWidget(true);
}

void UMM_PlayMenuWidget::OnQuickPlayClicked()
{
	/* Join an available session. If no sessions are available, create a new one. */
	if (IsValid(GameInstancePtr))
	{
		GameInstancePtr->JoinServer();
	}
}

void UMM_PlayMenuWidget::OnInviteFriendClicked()
{
	
}

void UMM_PlayMenuWidget::OnBackClicked()
{
	/* To navigate back to the main menu, queue the menu, which will deactivate this menu and transition to the new
	 * one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_MainMenu);
}

