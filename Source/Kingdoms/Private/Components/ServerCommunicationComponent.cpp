// Copyright Samuel Reitich 2022.


#include "Components/ServerCommunicationComponent.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "Pieces/PieceAIController.h"

#include "Kismet/KismetMathLibrary.h"

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

		/* Spawn a piece of a given class from the server. */
		AActor* SpawnedPiece = GetWorld()->SpawnActor
		(
			ClassToSpawn,
			&PieceSpawnLocation,
			&PieceSpawnRotation,
			SpawnParams
		);

		/* Play the pop-up animation for the actor. */
		if (const auto SpawnedPieceAsPiece = Cast<AParentPiece>(SpawnedPiece))
			SpawnedPieceAsPiece->Multicast_PlayPiecePopUp(0.15f, false);
		
		/* Add the spawned piece to this player's array of owned pieces. */
		OwningPlayerController->GetPlayerState<AMatch_PlayerState>()->OwnedPieces.Add(SpawnedPiece);
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
		Piece->Server_SetCurrentTile(NewTile);
		Cast<ABoardTile>(InNewTile)->SetOccupyingPiece(Piece);
	}
}

void UServerCommunicationComponent::Server_MovePieceToTile_Implementation(AParentPiece* PieceToMove,
	ABoardTile* NewTile, bool bResetStateWhenFinished)
{
	/* Rotate the piece towards its destination. When it finishes rotating, it calls MovePieceToTile. */
	Cast<AMatch_PlayerPawn>(PieceToMove->GetInstigator())->InterpolatePieceRotation(
		PieceToMove,
		NewTile,
		PieceToMove->GetActorRotation(),
		UKismetMathLibrary::FindLookAtRotation(PieceToMove->GetCurrentTile()->GetActorLocation(), NewTile->GetActorLocation()),
		true,
		true);
}

void UServerCommunicationComponent::MovePieceToTile(AParentPiece* PieceToMove, ABoardTile* NewTile, bool bResetStateWhenFinished)
{
	/* If the piece has a valid piece AI controller, use it to move the piece on the server. */
	if (IsValid(PieceToMove->GetController()) && PieceToMove->GetController()->GetClass()->IsChildOf(APieceAIController::StaticClass()))
	{
		/* This will reset the player's state when the piece finishes moving if bResetStateWhenFinished is true. */
		Cast<APieceAIController>(PieceToMove->GetController())->MovePieceToTile(NewTile, bResetStateWhenFinished);
	}

	/* Update the piece's new tile and the new tile's occupying piece on the server. */
	NewTile->SetOccupyingPiece(PieceToMove);
	PieceToMove->GetCurrentTile()->SetOccupyingPiece(nullptr);
	PieceToMove->Server_SetCurrentTile(NewTile);
}

void UServerCommunicationComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Get a reference to the player controller that owns this component. */
	OwningPlayerController = Cast<APlayerController>(GetOwner());
}
