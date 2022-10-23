// Copyright Samuel Reitich 2022.


#include "Board/BoardManager.h"

#include "Board/BoardTile.h"

#include "Kismet/GameplayStatics.h"

ABoardManager::ABoardManager()
{
 	/* Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = false;

}

void ABoardManager::BeginPlay()
{
	Super::BeginPlay();

	/* Automatically create the array of tiles in the correct order. */
	TArray<AActor*> AllTileActors;
	UGameplayStatics::GetAllActorsOfClass(this, ABoardTile::StaticClass(), AllTileActors);

	for (AActor* Tile : AllTileActors)
	{
		AllTiles.Add(Cast<ABoardTile>(Tile));
	}

	AllTiles.Sort();
}
