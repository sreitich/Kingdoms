// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Soldiers/Knight_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/Soldiers/Knight.h"
#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"

void UKnight_ActiveAbilityConfirmation::UpdateActionConfirmationInfo(AKnight* NewAbilityUser, ABoardTile* NewTargetTile)
{
	/* Store a reference to the ability's user if it's valid. */
	if (IsValid(NewAbilityUser))
		AbilityUser = NewAbilityUser;

	/* Store a reference to the new one if it's valid. */
	if (IsValid(NewTargetTile))
	{
		TargetTile = NewTargetTile;
	}
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
	/* Reset the player's selected piece and selected tile. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true, false, false, true, false);

	/* Reset the highlight of every tile that was highlighted. */
	if (IsValid(AbilityUser))
	{
		for (ABoardTile* Tile : AbilityUser->GetValidMoveTiles())
		{
			Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Activate this piece's active ability if there is a valid reference to it. */
	if (IsValid(AbilityUser))
	{
		const TArray<AActor*> Targets = { TargetTile };

		Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_UseActiveAbility(AbilityUser, Targets);
	}

	/* Clear these variables manually when this widget is destroyed. */
	TargetTile = nullptr;
	AbilityUser->ConfirmationWidget = nullptr;

	/* Destroy this widget after the ability is confirmed. */
	RemoveFromParent();
}

void UKnight_ActiveAbilityConfirmation::NativeDestruct()
{
	Super::NativeDestruct();

	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
}

void UKnight_ActiveAbilityConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
	/* Reset the player's selected piece and selected tile. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true, false, false, true, false);

	/* Reset the highlight of every tile that was highlighted. */
	if (IsValid(AbilityUser))
	{
		for (ABoardTile* Tile : AbilityUser->GetValidMoveTiles())
		{
			Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Nullify the piece's pointer to its ability confirmation widget so it makes a new one next time. */
	AbilityUser->ConfirmationWidget = nullptr;

	/* Destroy this widget. */
	RemoveFromParent();
}

