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

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* We need additional "fireball" ability code to execute before starting the attack. */
	virtual void OnAttackClicked() override;

	/* Overridden to clean up everything when the active ability is cancelled. */
	virtual void OnCancelClicked() override;
	
};
