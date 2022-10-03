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

	/* We need additional "fireball" ability code to execute before starting the attack. */
	virtual void OnAttackClicked() override;
	
};
