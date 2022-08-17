// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardManager.generated.h"

class ABoardTile;

UCLASS()
class KINGDOMS_API ABoardManager : public AActor
{
	GENERATED_BODY()

/* Public functions. */
public:
	
	/* Sets default values for this actor's properties. */
	ABoardManager();

	/* Refreshes the highlights of every tile on the board depending on their state on the server. */
	UFUNCTION(BlueprintCallable)
	void RefreshBoard() const;


/* Public variables. */
public:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Board")
	TArray<ABoardTile*> AllTiles;


protected:
	
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;
	
};
