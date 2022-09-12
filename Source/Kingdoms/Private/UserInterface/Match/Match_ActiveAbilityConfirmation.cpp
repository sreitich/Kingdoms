// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"

bool UMatch_ActiveAbilityConfirmation::SetAbilityUser(AParentPiece* NewAbilityUser)
{
	/* Update the ability user and return true if the given piece is valid. */
	if (IsValid(NewAbilityUser))
	{
		AbilityUser = NewAbilityUser;
		return true;
	}

	return false;
}

bool UMatch_ActiveAbilityConfirmation::SetPendingTargets(TArray<AActor*> NewTargets)
{
	/* Update the targets array and return true if ALL of the given pieces are valid. This also allows the array
	 * to be reset by passing an empty array, which will still pass this test. */
	bool AllActorsAreValid = true;
	
	for (const AActor* Actor : NewTargets)
	{
		if (!IsValid(Actor))
			AllActorsAreValid = false;
	}
	
	if (AllActorsAreValid)
		PendingTargets = NewTargets;

	return AllActorsAreValid;
}

bool UMatch_ActiveAbilityConfirmation::AddPendingTarget(AActor* NewTarget)
{
	/* Add the given target to the array if it's valid. */
	if (IsValid(NewTarget))
	{
		PendingTargets.Add(NewTarget);

		return true;
	}

	return false;
}

void UMatch_ActiveAbilityConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the confirm button to activate the piece's active ability. */
	ConfirmButton->OnClicked.AddDynamic(this, &UMatch_ActiveAbilityConfirmation::OnConfirmClicked);
	/* Bind the cancel button to reset the active ability action. */
	CancelButton->OnClicked.AddDynamic(this, &UMatch_ActiveAbilityConfirmation::OnCancelClicked);
}

void UMatch_ActiveAbilityConfirmation::OnConfirmClicked()
{
	/* Activate the selected piece's active ability if the selected piece is valid. */
	if (IsValid(AbilityUser))
		AbilityUser->OnActiveAbility(PendingTargets);
}

void UMatch_ActiveAbilityConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);

	/* Clear targeting indicators for every pending target (tile's highlights, pieces' tile's highlights, etc.) */
	for (AActor* Target : PendingTargets)
	{
		/* If the target was a tile, refresh its highlight. */
		if (ABoardTile* Tile = Cast<ABoardTile>(Target))
		{
			Tile->RefreshHighlight();
		}
		/* If the target was a piece, refresh that piece's tile's highlight. */
		else if (const AParentPiece* Piece = Cast<AParentPiece>(Target))
		{
			Piece->GetCurrentTile()->RefreshHighlight();
		}
		/* This can be iterated on if we add pieces that can target things other than tiles or pieces in the future. */
	}

	/* Destroy this widget. */
	RemoveFromParent();
}
