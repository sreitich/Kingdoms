// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Match_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMatch_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	AMatch_GameModeBase();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Choose the correct player start to spawn new players. */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


/* Private functions. */
private:

	/* Called when a player successfully logs into the game. Called after ChoosePlayerStart(). */
	virtual void PostLogin(APlayerController* NewPlayer) override;

};
