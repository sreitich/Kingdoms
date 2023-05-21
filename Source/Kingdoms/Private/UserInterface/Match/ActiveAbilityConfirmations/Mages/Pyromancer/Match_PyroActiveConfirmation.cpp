// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UMatch_PyroActiveConfirmation::DestroyWidget(bool bReset)
{
	AMatch_PlayerPawn* PlayerPawnPtr = GetOwningPlayerPawn<AMatch_PlayerPawn>();
	
	/* Reset the player state. */
	PlayerPawnPtr->GetPlayerState<AMatch_PlayerState>()->SetPlayerStatus(E_SelectingPiece);

	/* Reset the highlight of every tile that was highlighted. */
	if (IsValid(PendingFriendlyPiece))
	{
		for (ABoardTile* Tile : PendingFriendlyPiece->GetActiveAbilityRange())
		{
			Tile->UpdateHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Clear the player's selected piece, selected enemy piece, and selected target and hide the piece info widgets. */
	PlayerPawnPtr->ClearSelection(true, true, true, false, true);

	/* Nullify the piece's pointer to its active ability confirmation widget so it makes a new one next time. */
	PendingFriendlyPiece->ActiveAbilityConfirmationWidget = nullptr;
	
	/* Clear this widget's information and disable the confirmation button. */
	PendingFriendlyPiece = nullptr;
	PendingEnemyPiece = nullptr;
	AttackButton->SetIsEnabled(false);

	/* Destroy this widget. */
	RemoveFromParent();
}

void UMatch_PyroActiveConfirmation::OnAttackClicked()
{
	/* Activate this piece's active ability if there is a valid reference to it. */
	if (IsValid(PendingFriendlyPiece))
	{
		const TArray<AActor*> Targets = { PendingEnemyPiece };
	
		Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_UseActiveAbility(PendingFriendlyPiece, Targets);
	}

	/* Destroy this widget and reset the player and all tiles. */
	DestroyWidget(true);
}

void UMatch_PyroActiveConfirmation::OnCancelClicked()
{
	/* Destroy this widget and reset the player and all tiles. */
	DestroyWidget(true);
}
