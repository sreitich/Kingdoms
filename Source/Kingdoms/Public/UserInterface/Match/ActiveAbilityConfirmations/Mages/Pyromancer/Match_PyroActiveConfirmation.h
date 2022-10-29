// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "UserInterface/Match/Match_AttackConfirmation.h"
#include "Match_PyroActiveConfirmation.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_PyroActiveConfirmation : public UMatch_AttackConfirmation
{
	GENERATED_BODY()

/* Public variables. */
public:

	UPROPERTY()
	class APyromancer* ConfirmingPiece = nullptr;
	
/* Protected functions. */
protected:

	/* Bind the Pyromancer's OnAttackClicked and OnCancelClicked functions to the "Attack" and "Cancel" buttons,
	 * overriding the default functions that are bound by the attack confirmation widget. */
	virtual void BindButtons() override;

	/* We need additional "fireball" ability code to execute before starting the attack. */
	virtual void OnAttackClicked() override;

	/* Overridden to clean up everything when the active ability is cancelled. */
	virtual void OnCancelClicked() override;
	
};
