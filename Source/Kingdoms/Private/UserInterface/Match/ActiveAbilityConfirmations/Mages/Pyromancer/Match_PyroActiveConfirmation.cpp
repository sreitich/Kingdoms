// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Framework/Match/Match_PlayerPawn.h"
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
