// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "UserInterface/Match/Match_ActiveAbilityConfirmation.h"
#include "King_ActiveAbilityConfirmation.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UKing_ActiveAbilityConfirmation : public UMatch_ActiveAbilityConfirmation
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Reset the player's state and deselect all friendly pieces. */
	virtual void OnCancelClicked() override;
};
