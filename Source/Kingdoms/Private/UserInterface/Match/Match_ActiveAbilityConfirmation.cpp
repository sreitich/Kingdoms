// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"

void UMatch_ActiveAbilityConfirmation::Widget_UpdateActiveConfirmation(AParentPiece* NewPieceUsingAbility, TArray<AActor*> Targets)
{
	/* Update the user of the ability this widget is confirming, if the given one is valid. */
	if (IsValid(NewPieceUsingAbility))	
		PieceUsingAbility = NewPieceUsingAbility;

	/* Check if at least one target was given and that are given targets are valid. */
	bool bValidTarget = true;
	if (Targets.Num() > 0)
	{
		for (const AActor* Target : Targets)
			if (!IsValid(Target))
				bValidTarget = false;
	}
	else
	{
		bValidTarget = false;
	}

	/* Update the targets of the ability that this widget is confirming and enable the confirmation button. */
	if (bValidTarget)
	{
		PendingTargets = Targets;

		ConfirmButton->SetIsEnabled(true);
	}
	/* Disable the confirmation button if invalid targets were given. */
	else
	{
		ConfirmButton->SetIsEnabled(false);
	}
}

bool UMatch_ActiveAbilityConfirmation::SetPieceUsingAbility(AParentPiece* NewPieceUsingAbility)
{
	/* Update the ability user and return true if the given piece is valid. */
	if (IsValid(NewPieceUsingAbility))
	{
		PieceUsingAbility = NewPieceUsingAbility;
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

void UMatch_ActiveAbilityConfirmation::DestroyWidget()
{
	AMatch_PlayerPawn* PlayerPawnPtr = GetOwningPlayerPawn<AMatch_PlayerPawn>();

	/* Reset the player state. */
	PlayerPawnPtr->GetPlayerState<AMatch_PlayerState>()->SetPlayerStatus(E_SelectingPiece);

	/* Reset the highlight of every tile that was highlighted. */
	if (IsValid(PieceUsingAbility))
	{
		for (ABoardTile* Tile : PieceUsingAbility->GetActiveAbilityRange())
		{
			Tile->UpdateEmissiveHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Reset the player's selected piece, enemy piece, target piece and tile and hide the piece info widgets. */
	// Might not want to clear the selected piece and enemy piece
    PlayerPawnPtr->ClearSelection(true, true, true, true, true);

	/* Nullify the piece's pointer to its active ability confirmation widget so it makes a new one next time. */
	PieceUsingAbility->ActiveAbilityConfirmationWidget = nullptr;

	/* Clear this widget's information and disable the confirmation button. */
	PieceUsingAbility = nullptr;
	PendingTargets = TArray<AActor*>();
	ConfirmButton->SetIsEnabled(false);

	/* Destroy this widget. */
	RemoveFromParent();
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
	if (IsValid(PieceUsingAbility))
		Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn())->Server_UseActiveAbility(PieceUsingAbility, PendingTargets);

	/* Destroy this widget and reset the player and all tiles. */
	DestroyWidget();
}

void UMatch_ActiveAbilityConfirmation::OnCancelClicked()
{
	// /* Clear targeting indicators the target depending on what class it was. */
	//  for (AActor* ValidTarget : PieceUsingAbility->GetValidActiveAbilityTargets())
	//  {
	// 	/* If the target was a tile, reset its highlight. */
	// 	if (ABoardTile* Tile = Cast<ABoardTile>(ValidTarget))
	// 	{
	// 		Tile->UpdateEmissiveHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
	// 	}
	// 	/* If the target was a piece, reset that piece's tile's highlight. */
	// 	else if (const AParentPiece* Piece = Cast<AParentPiece>(ValidTarget))
	// 	{
	// 		Piece->GetCurrentTile()->UpdateEmissiveHighlight(false, Tile->DefaultHighlightPlayRate, Tile->EmissiveHighlight->GetLightColor());
	// 	}
	// 	/* This can be iterated on if we add pieces that can target things other than tiles or pieces in the future. */
	//
	
	/* Destroy this widget and reset the player and all tiles. */
	DestroyWidget();
}
