// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Soldiers/Knight_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/Soldiers/Knight.h"

void UKnight_ActiveAbilityConfirmation::UpdateActionConfirmationInfo(AKnight* NewAbilityUser,
	ABoardTile* NewTargetTile)
{
	/* Store a reference to the ability's user and target if they are valid. */
	if (IsValid(NewAbilityUser))
		AbilityUser = NewAbilityUser;

	if (IsValid(NewTargetTile))
		TargetTile = NewTargetTile;
		
}

void UKnight_ActiveAbilityConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the confirm button to activate the piece's active ability. */
	ConfirmButton->OnClicked.AddDynamic(this, &UKnight_ActiveAbilityConfirmation::OnConfirmClicked);
	/* Bind the cancel button to reset the active ability action. */
	CancelButton->OnClicked.AddDynamic(this, &UKnight_ActiveAbilityConfirmation::OnCancelClicked);
}

void UKnight_ActiveAbilityConfirmation::OnConfirmClicked()
{
	/* Activate this piece's active ability if there is a valid reference to it. */
	if (IsValid(AbilityUser))
	{
		const TArray<AActor*> Targets = { TargetTile };

		// AbilityUser->OnActiveAbility(Targets);

		Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_UseActiveAbility(AbilityUser, Targets);
	}
}

void UKnight_ActiveAbilityConfirmation::NativeDestruct()
{
	Super::NativeDestruct();

	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);

	/* Clear the targeting indicators on the valid target tiles. */
	for (AActor* ValidTarget : AbilityUser->GetValidActiveAbilityTargets())
	{
		/* If the target was a tile (which it should always be), refresh its highlight. */
		if (ABoardTile* Tile = Cast<ABoardTile>(ValidTarget))
		{
			Tile->RefreshHighlight();
		}
	}
}

void UKnight_ActiveAbilityConfirmation::OnCancelClicked()
{
	/* Clean up and destroy this widget. */
	NativeDestruct();
}

