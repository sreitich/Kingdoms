// Copyright Samuel Reitich 2022.


#include "Board/BoardManager.h"

#include "Board/BoardTile.h"

#include "Kismet/GameplayStatics.h"

ABoardManager::ABoardManager()
{
 	/* Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = false;

}

void ABoardManager::RefreshBoard() const
{
	/* Refresh every tile's displayed highlight for the client that called the function. */
	for (ABoardTile* Tile : AllTiles)
	{
		Tile->RefreshHighlight();
	}
}

void ABoardManager::BeginPlay()
{
	Super::BeginPlay();

	/* If I haven't set the array of every board tile, get all of the tiles in the level and add them to the array.
	 * Warning: This runs the risk of the tiles being out of order. */
	if (AllTiles.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AllTiles for %s is not set."), *GetName());
		
		TArray<AActor*> AllTileActors;
		UGameplayStatics::GetAllActorsOfClass(this, ABoardTile::StaticClass(), AllTileActors);
	
		for (AActor* Tile : AllTileActors)
		{
			AllTiles.Add(Cast<ABoardTile>(Tile));
		}
	}
	
}
