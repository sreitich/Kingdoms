// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"

void UMatch_ActiveAbilityConfirmation::UpdateActionConfirmationInfo(AParentPiece* NewAbilityUser, AActor* Target)
{
	/* Update the user of the ability this widget is confirming, if the given one is valid. */
	if (IsValid(NewAbilityUser))	
		AbilityUser = NewAbilityUser;

	/* Set this ability's target. */
	PendingTarget = Target;
}

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

bool UMatch_ActiveAbilityConfirmation::SetPendingTarget(AActor* NewTarget)
{
	/* Update the pending target and return true if the given target is valid. */
	if (IsValid(NewTarget))
	{
		PendingTarget = NewTarget;
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
		AbilityUser->OnActiveAbility(PendingTarget);

	/* Act as if the widget was cancelled, resetting the player's state and removing all highlighted valid targets.*/
	OnCancelClicked();
}

void UMatch_ActiveAbilityConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);

	/* Clear targeting indicators the target depending on what class it was. */
    for (AActor* ValidTarget : AbilityUser->GetValidActiveAbilityTargets())
    {
		/* If the target was a tile, refresh its highlight. */
		if (ABoardTile* Tile = Cast<ABoardTile>(ValidTarget))
		{
			Tile->RefreshHighlight();
		}
		/* If the target was a piece, refresh that piece's tile's highlight. */
		else if (const AParentPiece* Piece = Cast<AParentPiece>(ValidTarget))
		{
			Piece->GetCurrentTile()->RefreshHighlight();
		}
		/* This can be iterated on if we add pieces that can target things other than tiles or pieces in the future. */
    }

	/* Destroy this widget. */
	RemoveFromParent();
}
