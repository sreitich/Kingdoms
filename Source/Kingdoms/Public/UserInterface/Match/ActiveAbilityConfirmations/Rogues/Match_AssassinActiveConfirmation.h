// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "UserInterface/Match/Match_AttackConfirmation.h"
#include "Match_AssassinActiveConfirmation.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_AssassinActiveConfirmation : public UMatch_AttackConfirmation
{
	GENERATED_BODY()

/* Protected functions. */
protected:

	/* Cleans up and destroys this widget. Resets the player and all tiles if bReset is true. Since this is actually an
	 * active ability confirmation widget, bReset will never be false. */
	virtual void DestroyWidget(bool bReset) override;


	/* We need to override both of these functions because this widget inherits from the attack confirmation widget, not
	 * the active ability confirmation widget. */
	
	/* We need additional "fireball" ability code to execute before starting the attack. */
	virtual void OnAttackClicked() override;

	/* Destroys this widget and resets the player and all tiles. */
	virtual void OnCancelClicked() override;
	
};
