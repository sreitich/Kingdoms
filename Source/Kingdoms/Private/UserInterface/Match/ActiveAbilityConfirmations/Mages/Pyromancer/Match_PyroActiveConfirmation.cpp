// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Animations/AnimInstance_Parent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Pieces/ParentPiece.h"
#include "Pieces/Mages/Pyromancer.h"

void UMatch_PyroActiveConfirmation::OnAttackClicked()
{
	/* Zoom each player's camera into the fight
	 * Set attack animation to be the power-up animation.
	 * In the power-up animation, increase the piece's strength and make a strength buff pop-up
	 * When the animation ends, reset the attack animation variable.
	 * Change the pyromancer's attack animation.
	 * Start the attack sequence.
	 */

	for (const APlayerState* PlayerStatePtr : UGameplayStatics::GetGameState(this)->PlayerArray)
	{
		/* Zoom each player into the fight. */
		if (AMatch_PlayerPawn* PlayerPawnPtr = Cast<AMatch_PlayerPawn>(PlayerStatePtr->GetPawn()))
			PlayerPawnPtr->Client_MovePlayerCamera(PendingFriendlyPiece, PendingEnemyPiece, false);

		/* Play the power-up animation. */
		if (UAnimInstance_Parent* AnimInstance = Cast<UAnimInstance_Parent>(PendingFriendlyPiece->GetMesh()->GetAnimInstance()))
		{
			AnimInstance->ActiveAbilityAnimation = Cast<APyromancer>(PendingFriendlyPiece)->PowerUpAnimation;
			AnimInstance->bUsingActive = true;
		}
	}
	
	// Super::OnAttackClicked();
}
