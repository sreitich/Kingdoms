// Copyright Samuel Reitich 2022.


#include "Components/ServerCommunicationComponent.h"

#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Pieces/PieceAIController.h"

UServerCommunicationComponent::UServerCommunicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UServerCommunicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UServerCommunicationComponent::SpawnPiece_Server_Implementation(UClass* ClassToSpawn, FVector PieceSpawnLocation, FRotator PieceSpawnRotation, UPieceDragWidget* PieceDragWidgetRef)
{
	/* This component needs to be owned by a player controller that possesses a pawn in order to work. */
	if (OwningPlayerController && OwningPlayerController->GetPawn())
	{
		/* Define additional spawn parameters. */
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = OwningPlayerController->GetPawn();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		/* Spawn a piece of a given class from the server and add it to this player's array of owned pieces. */
		OwningPlayerController->GetPlayerState<AMatch_PlayerState>()->OwnedPieces.Add(GetWorld()->SpawnActor
		(
			ClassToSpawn,
			&PieceSpawnLocation,
			&PieceSpawnRotation,
			SpawnParams
		));

		UE_LOG(LogTemp, Error, TEXT("Spawned piece: %s"), *ClassToSpawn->GetName());
	}
}

void UServerCommunicationComponent::UpdatePiecePosition_Server_Implementation(AActor* InPiece, AActor* InNewTile)
{
	/* If both the piece and new tile aren't null. */
	if (IsValid(InPiece) && IsValid(InNewTile))
	{
		/* Get a casted reference to the piece and its new tile. */
		AParentPiece* Piece = Cast<AParentPiece>(InPiece);
		ABoardTile* NewTile = Cast<ABoardTile>(InNewTile);
		
		/* If the piece already had a current tile set... */
		if (Piece->GetCurrentTile())
		{
			/* Make the old tile unoccupied. */
			Piece->GetCurrentTile()->SetOccupyingPiece(nullptr);
		}

		/* Update the piece's location on the server. */
		Piece->SetActorLocation(InNewTile->GetActorLocation() + FVector(0.0f, 0.0f, 165.0f));

		/* Set the piece's new current tile and the tile's new occupying piece. */
		Piece->SetCurrentTile(NewTile);
		Cast<ABoardTile>(InNewTile)->SetOccupyingPiece(Piece);
	}
}

void UServerCommunicationComponent::MovePieceToTile_Server_Implementation(AParentPiece* PieceToMove,
	ABoardTile* NewTile)
{
	/* If the piece has a valid piece AI controller, use it to move the piece on the server. */
	if (IsValid(PieceToMove->GetController()) && PieceToMove->GetController()->GetClass()->IsChildOf(APieceAIController::StaticClass()))
	{
		Cast<APieceAIController>(PieceToMove->GetController())->MovePieceToTile(NewTile);
	}

	/* Update the piece's new tile and the new tile's occupying piece on the server. */
	PieceToMove->GetCurrentTile()->SetOccupyingPiece(nullptr);
	PieceToMove->SetCurrentTile(NewTile);
	NewTile->SetOccupyingPiece(PieceToMove);
	
	/* Reset the player's player state. */
	Cast<AMatch_PlayerController>(OwningPlayerController)->GetPlayerState<AMatch_PlayerState>()->SetPlayerStatus_Server(E_SelectingPiece);
}

void UServerCommunicationComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Get a reference to the player controller that owns this component. */
	OwningPlayerController = Cast<APlayerController>(GetOwner());
}
