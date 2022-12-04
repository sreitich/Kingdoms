// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"

void UMatch_ActiveAbilityConfirmation::UpdateActionConfirmationInfo(AParentPiece* NewAbilityUser, TArray<AActor*> Targets)
{
	/* Update the user of the ability this widget is confirming, if the given one is valid. */
	if (IsValid(NewAbilityUser))	
		AbilityUser = NewAbilityUser;

	/* Set this ability's target. */
	PendingTargets = Targets;
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

bool UMatch_ActiveAbilityConfirmation::SetPendingTargets(TArray<AActor*> NewTargets)
{
	/* Ensure that all of the given targets are valid. */
	bool bIsValid = true;
	for (const AActor* ActorPtr : NewTargets)
	{
		if (!IsValid(ActorPtr))
		{
			bIsValid = false;
			break;
		}
	}

	/* Update the pending target and return true if the given targets are valid. */
	if (bIsValid)
	{
		PendingTargets = NewTargets;
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
	/* Activate the selected piece's active ability from the server if there is a valid reference to it. */
	if (IsValid(AbilityUser))
		Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_UseActiveAbility(AbilityUser, PendingTargets);

	/* Act as if the widget was cancelled, resetting the player's state and removing all highlighted valid targets.*/
	OnCancelClicked();
}

void UMatch_ActiveAbilityConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
	/* Reset the player's selected piece, selected target piece, and selected tile. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true, false, true, true, false);

	/* Clear targeting indicators the target depending on what class it was. */
    for (AActor* ValidTarget : AbilityUser->GetValidActiveAbilityTargets())
    {
		/* If the target was a tile, reset its highlight. */
		if (ABoardTile* Tile = Cast<ABoardTile>(ValidTarget))
		{
			Tile->UpdateEmissiveHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
		}
		/* If the target was a piece, reset that piece's tile's highlight. */
		else if (const AParentPiece* Piece = Cast<AParentPiece>(ValidTarget))
		{
			Piece->GetCurrentTile()->UpdateEmissiveHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
		}
		/* This can be iterated on if we add pieces that can target things other than tiles or pieces in the future. */
    }

	/* Nullify the piece's pointer to its active ability confirmation widget so it makes a new one next time. */
	AbilityUser->ActiveAbilityConfirmationWidget = nullptr;

	/* Destroy this widget. */
	RemoveFromParent();
}
