// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Pieces/ParentPiece.h"
#include "Pieces/Mages/Pyromancer.h"

void UMatch_PyroActiveConfirmation::OnAttackClicked()
{
	/* Activate this piece's active ability if there is a valid reference to it. */
	if (IsValid(PendingFriendlyPiece))
	{
		const TArray<AActor*> Targets = { PendingEnemyPiece };
	
		Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_UseActiveAbility(PendingFriendlyPiece, Targets);
	}

	ConfirmingPiece->ConfirmationWidget = nullptr;
	
	/* Destroy this widget after the ability is confirmed. */
	RemoveFromParent();
}

void UMatch_PyroActiveConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);

	/* For every tile that was highlighted... */
	for (AActor* Tile : PendingFriendlyPiece->GetActiveAbilityRange())
	{
		/* Refresh the tile's highlight depending on its occupying piece to clear the highlights. */
		Cast<ABoardTile>(Tile)->RefreshHighlight();
	}

	/* Nullify the piece's pointer to its ability confirmation widget so it makes a new one next time. */
	Cast<APyromancer>(PendingFriendlyPiece)->ConfirmationWidget = nullptr;

	/* Destroy this widget. */
	RemoveFromParent();
}
