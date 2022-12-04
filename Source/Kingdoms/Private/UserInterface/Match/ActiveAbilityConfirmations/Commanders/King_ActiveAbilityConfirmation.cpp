// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Commanders/King_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"

#include "Components/RectLightComponent.h"

void UKing_ActiveAbilityConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
	/* Reset the player's selected piece, selected target piece, and selected tile. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true, false, true, true, false);
	/* Nullify the piece's pointer to its active ability confirmation widget so it makes a new one next time. */
	AbilityUser->ActiveAbilityConfirmationWidget = nullptr;

	/* Remove the highlights from all friendly pieces and their tiles. */
	for (AActor* Target : AbilityUser->GetValidActiveAbilityTargets())
	{
		if (AParentPiece* Piece = Cast<AParentPiece>(Target))
		{
			/* Reset each piece's highlight. */
			Piece->FlashHighlight(Piece->FriendlyFresnelColor, Piece->DefaultFresnelStrength, 1.0f, 0.0f, true);

			/* Reset each tile's highlight. */
			ABoardTile* Tile = Piece->GetCurrentTile();
			Tile->UpdateEmissiveHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Destroy this widget. */
	RemoveFromParent();
}
