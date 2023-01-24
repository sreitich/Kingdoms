// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Match_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_GameInstance : public UGameInstance
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Attempts to create and join a brand new session. Returns true if the new session was successfully created. */
	UFUNCTION()
	bool TryCreateSession();

	/* Attempts to join an existing online session. Returns true if a session was successfully found.*/
	UFUNCTION()
	bool TryJoinSession();
	
};
