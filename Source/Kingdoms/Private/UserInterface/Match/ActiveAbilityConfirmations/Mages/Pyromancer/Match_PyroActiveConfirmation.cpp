// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "Pieces/Mages/Pyromancer.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UMatch_PyroActiveConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the confirm button to activate the piece's active ability. */
	AttackButton->OnClicked.AddDynamic(this, &UMatch_PyroActiveConfirmation::OnAttackClicked);
	/* Bind the cancel button to reset the active ability action. */
	CancelButton->OnClicked.AddDynamic(this, &UMatch_PyroActiveConfirmation::OnCancelClicked);
}

void UMatch_PyroActiveConfirmation::OnAttackClicked()
{
	/* Clear the player's selected piece. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true);

	/* Reset the highlight of every tile that was highlighted. */
	for (AActor* Target : PendingFriendlyPiece->GetValidActiveAbilityTargets())
	{
		if (ABoardTile* Tile = Cast<ABoardTile>(Target))
			Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
	}

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
	/* Nullify the piece's pointer to its ability confirmation widget so it makes a new one next time. */
	Cast<APyromancer>(PendingFriendlyPiece)->ConfirmationWidget = nullptr;

	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
	/* Clear the player's selected piece. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true);

	if (IsValid(PendingFriendlyPiece))
	{
		/* Reset the highlight of every tile that was highlighted. */
		for (AActor* Target : PendingFriendlyPiece->GetValidActiveAbilityTargets())
		{
			if (ABoardTile* Tile = Cast<ABoardTile>(Target))
				Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Nullify the piece's pointer to its ability confirmation widget so it makes a new one next time. */
	// Cast<APyromancer>(PendingFriendlyPiece)->ConfirmationWidget = nullptr;

	/* Destroy this widget. */
	RemoveFromParent();
}
