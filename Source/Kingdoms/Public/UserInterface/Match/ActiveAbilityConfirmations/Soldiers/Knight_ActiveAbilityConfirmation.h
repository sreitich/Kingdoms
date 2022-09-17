// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Knight_ActiveAbilityConfirmation.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UKnight_ActiveAbilityConfirmation : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

};
