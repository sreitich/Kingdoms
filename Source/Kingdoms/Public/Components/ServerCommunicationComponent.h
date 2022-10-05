// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerCommunicationComponent.generated.h"

class UPieceDragWidget;
class AParentPiece;
class ABoardTile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KINGDOMS_API UServerCommunicationComponent : public UActorComponent
{
	GENERATED_BODY()

/* Public functions. */
public:
	
	/* Sets default values for this component's properties. */
	UServerCommunicationComponent();

	/* Called every frame. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Called from the drag widget to spawn a new piece of the given class at the given location. */
	UFUNCTION(Server, Reliable)
	void SpawnPiece_Server(UClass* ClassToSpawn, FVector PieceSpawnLocation, FRotator PieceSpawnRotation, UPieceDragWidget* PieceDragWidgetRef);

	/* Called when the drag widget is destroyed to update the new position of the piece on the server and update its new tile. */
	UFUNCTION(Server, Reliable)
	void UpdatePiecePosition_Server(AActor* InPiece, AActor* InNewTile);

	/* Rotates the piece to a target tile destination and calls MovePieceToTile when completed. */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_MovePieceToTile(AParentPiece* PieceToMove, ABoardTile* NewTile, bool bResetStateWhenFinished);

	/* Moves a piece to a new tile on the server, while also updating relevant actor data, including current tile and occupying piece references.
	If bFromMove, the player state will be reset to E_SelectingPiece. Called after Server_MovePieceToTile() finishes rotating the target piece. */
		UFUNCTION(BlueprintCallable)
		void MovePieceToTile(AParentPiece* PieceToMove, ABoardTile* NewTile, bool bResetStateWhenFinished);
	

/* Protected functions. */
protected:

	/* Called when the game . */
	virtual void BeginPlay() override;


/* Protected variables. */
protected:

	/* A reference to the player controller that owns this component. */
	UPROPERTY()
	APlayerController* OwningPlayerController = nullptr;

		
};
