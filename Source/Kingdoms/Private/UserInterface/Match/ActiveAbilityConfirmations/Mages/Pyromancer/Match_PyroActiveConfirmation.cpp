// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

void UMatch_PyroActiveConfirmation::OnAttackClicked()
{
	/* Zoom each player's camera into the fight
	 * Set attack animation to be the power-up animation.
	 * In the power-up animation, increase the piece's strength and make a strength buff pop-up
	 * When the animation ends, reset the attack animation variable.
	 * Change the pyromancer's attack animation.
	 * Start the attack sequence.
	 */
	
	Super::OnAttackClicked();
}
