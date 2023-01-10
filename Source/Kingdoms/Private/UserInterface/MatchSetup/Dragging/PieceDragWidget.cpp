// Copyright Change Studios, LLC 2023.


#include "UserInterface/MatchSetup/Dragging/PieceDragWidget.h"

#include "Board/BoardTile.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Pieces/ParentPiece.h"
#include "UserInterface/MatchSetup/MatchSetup_PlacePieces.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#define OUT

void UPieceDragWidget::InitializeWidget(bool bSpawnPiece)
{
	/* If this widget's piece needs to be spawned... */
	if (bSpawnPiece)
	{
		/* Hide this widget with an animation. */
		PlayAnimation(TransitionToPieceAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
		
		/* The location and rotation to spawn this widget's corresponding piece at. */
		FVector PieceSpawnLocation = FVector(0.0f, 0.0f, 165.0f);
		FRotator PieceSpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

		/* Perform a line trace for board tiles to the location of the player's mouse. */
		FHitResult HitResult = TraceFromMouse();
			
		/* If the trace hit a board tile... */
		if (HitResult.bBlockingHit)
		{
			/* If the tile is empty, set the new piece's spawn location to 165 units above the hovered tile. */
			if (!IsValid(Cast<ABoardTile>(HitResult.GetActor())->GetOccupyingPiece()))
			{
				/* Set the new piece's spawn location to 165 units above the hovered tile. */
				PieceSpawnLocation = HitResult.GetActor()->GetActorLocation() + FVector(0.0f, 0.0f, 165.0f);
			}
		}
		/* If the trace didn't hit a tile, set the new piece's spawn location to the intersection of the trace and board tile plane, so that it is in line with the tiles. */
		else
		{
			/* Set the new piece's spawn location to the tile closest to where the player clicked along the plane. */
			PieceSpawnLocation = GetClosestOpenTile(HitResult.ImpactPoint)->GetActorLocation() + FVector(0.0f, 0.0f, 165.0f);
		}

		/* Get the direction of this player's pawn so that the placed piece faces the same direction as its owning player. */
		PieceSpawnRotation.Yaw = GetOwningPlayerPawn()->GetActorRotation().Yaw;

		/* Spawn the new piece on the server that will replicate. */
		GetOwningPlayer<AMatch_PlayerController>()->GetServerCommunicationComponent()->SpawnPiece_Server(ClassToSpawn, PieceSpawnLocation, PieceSpawnRotation, this);

		/* Get a board tile and get the universal height of tiles on this level. */
		const AActor* BoardTile = UGameplayStatics::GetActorOfClass(GetWorld(), ABoardTile::StaticClass());
		BoardTileHeight = BoardTile->GetActorLocation().Z;
	}
	/* If this widget's piece has already been spawned, then the piece just needs to be moved, and no new actors need to be spawned. */
	else
	{
		/* This widget needs to be in the viewport and cannot be hidden for it to function, but it doesn't need to be directly interacted with. So we can just make it invisible and unable to be interacted with. */
		SetRenderOpacity(0.0f);
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	/* Get the PlacePieces widget and hide the remaining cards so they don't block any board tiles. */
	TArray<UUserWidget*> PlacePiecesWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), OUT PlacePiecesWidgets, UMatchSetup_PlacePieces::StaticClass(), true);
	if (PlacePiecesWidgets.Num() > 0)
	{
		Cast<UMatchSetup_PlacePieces>(PlacePiecesWidgets[0])->PlayHideCardsAnim(false);
	}
}

void UPieceDragWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the TransitionAnimFinished function to when the transition animation finishes playing. */
	FinishedDelegate.BindDynamic(this, &UPieceDragWidget::TransitionAnimFinished);
	
	/* Bind a function to hide this widget when the transition animation finishes playing. */
	BindToAnimationFinished(TransitionToPieceAnim, FinishedDelegate);
}

void UPieceDragWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	/* Try to destroy this widget again if it's safe to. */
	if (bPendingDestruction)
	{
		NativeDestruct();
	}
	else
	{
		/* If the piece has been spawned yet, which is always should be... */
		if (IsValid(SpawnedPiece))
		{
			/* Perform a line trace for board tiles */
			FHitResult HitResult = TraceFromMouse();
			
			/* If the trace hit a board tile... */
			if (HitResult.bBlockingHit)
			{
				/* If the tile is empty, snap the piece to 165 units above the hit tile. If the tile already has a piece and isn't the dragged piece's current tile, don't move the dragged piece. */
				if (!IsValid(Cast<ABoardTile>(HitResult.GetActor())->GetOccupyingPiece()) || SpawnedPiece->GetCurrentTile() == HitResult.GetActor())
				{
					/* Snap the spawned piece to 165 units above the hit tile. */
					SpawnedPiece->SetActorLocation(HitResult.GetActor()->GetActorLocation() + FVector(0.0f, 0.0f, 165.0f));

					/* Set this piece's new board tile. */
					CurrentTile = HitResult.GetActor();

					/* If the tile actually needs to be updated. */
					if (SpawnedPiece->GetCurrentTile() != CurrentTile)
					{
						/* Update the dragged piece's position on the server and its new tile. */
						GetOwningPlayer<AMatch_PlayerController>()->GetServerCommunicationComponent()->UpdatePiecePosition_Server(SpawnedPiece, CurrentTile);
					}
				}
			}
		}
		/* If this widget doesn't have a reference to the new piece yet... */
		else
		{
			/* Get this widget's owning pawn. */
			AMatch_PlayerPawn* PlayerPawn = Cast<AMatch_PlayerPawn>(GetOwningPlayerPawn());
		
			/* If the pawn is of the correct class, and the new piece has been spawned... */
			if (PlayerPawn && PlayerPawn->DraggedPiece)
			{
				/* Get a reference to the newly spawned piece. */
				SpawnedPiece = Cast<AParentPiece>(PlayerPawn->DraggedPiece);

				/* Clear the pawn's DraggedPiece so that it isn't assigned to the next drag widget by mistake. */
				PlayerPawn->DraggedPiece = nullptr;
			}
		}
	}
}

void UPieceDragWidget::NativeDestruct()
{
	Super::NativeDestruct();

	/* The server needs this widget's data to spawn the piece, so it can't be destroyed before the server finished spawning the piece. */
	if (IsValid(SpawnedPiece))
	{
		/* Get the PlacePieces widget and return the remaining cards to the screen. */
		TArray<UUserWidget*> PlacePiecesWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), OUT PlacePiecesWidgets, UMatchSetup_PlacePieces::StaticClass(), true);
		if (PlacePiecesWidgets.Num() > 0)
		{
			Cast<UMatchSetup_PlacePieces>(PlacePiecesWidgets[0])->PlayHideCardsAnim(true);
		}

		/* If the original widget that this widget represents was set, then it was created by an unplaced piece widget that needs to call the following additional logic when destroyed. */
		if (RepresentedWidget)
		{
			/* Remove the original widget from the unplaced pieces box. */
			RepresentedWidget->RemoveFromParent();

			/* If all of this player's pieces have been placed, allow them to declare that they are ready. */
			Cast<UMatchSetup_PlacePieces>(PlacePiecesWidgets[0])->CheckAllPiecesPlaced();
		}

		/* If the piece was placed off the board... */
		if (!IsValid(CurrentTile))
		{
			/* Set the piece's current tile to be the closest available board tile to it. */
			CurrentTile = GetClosestOpenTile(SpawnedPiece->GetActorLocation());
		}

		/* Update the dragged piece's position on the server and its new tile, if it needs to be. */
		if (SpawnedPiece->GetCurrentTile() != CurrentTile)
		{
			GetOwningPlayer<AMatch_PlayerController>()->GetServerCommunicationComponent()->UpdatePiecePosition_Server(SpawnedPiece, CurrentTile);
		}
	}
	else
	{
		bPendingDestruction = true;
	}
}

void UPieceDragWidget::TransitionAnimFinished()
{
	/* Hide this widget. The widget has to remain hit detectable to continue tracking the drag. */
	SetRenderOpacity(0.0f);
}

FHitResult UPieceDragWidget::TraceFromMouse() const
{
	/* Declare variables needed for a line trace. */
	FVector WorldLocation, WorldDirection;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;

	/* Get the position of the player's mouse scaled by the viewport. */
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());

	/* Convert the mouse's position into a traceable direction in the world. */
	GetOwningPlayer()->DeprojectScreenPositionToWorld(MousePos.X, MousePos.Y, OUT WorldLocation, OUT WorldDirection);

	/* Perform a line trace to the location of the player's mouse, only returning board tiles. */
	GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResult,
		WorldLocation,
		WorldLocation + (WorldDirection * 3000.0f),
		ECC_GameTraceChannel1, /* "BoardTile" trace channel so that only board tiles are hit. */
		TraceParams
	);

	/* If the trace hit a board tile... */
	if (HitResult.bBlockingHit)
	{
		/* If the tile is empty,  */
	}
	/* If the trace didn't hit anything, then the player didn't hover over a tile, and instead needs an intersection to the tile plane. */
	else
	{
		/* Set the hit result's impact point to the intersection of the trace and tile plane to use later. */
		HitResult.ImpactPoint = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + (WorldDirection * 3000.0f), FVector(0.0f, 0.0f, BoardTileHeight), FVector(0.0f, 0.0f, 1.0f));
	}

	return HitResult;
}

AActor* UPieceDragWidget::GetClosestOpenTile(const FVector& CloseToLocation) const
{
	/* Get every board tile. */
	TArray<AActor*> AllTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoardTile::StaticClass(), OUT AllTiles);
	
	/* Get the closest tile to the given location */
	float DistanceToClosestTile = FVector::Distance(AllTiles[0]->GetActorLocation(), CloseToLocation);
	AActor* ClosestTile = AllTiles[0];
		
	for (AActor* Tile : AllTiles)
	{
		/* Also make sure that this tile isn't occupied. */
		if (FVector::Distance(Tile->GetActorLocation(), CloseToLocation) < DistanceToClosestTile && !IsValid(Cast<ABoardTile>(Tile)->GetOccupyingPiece()))
		{
			DistanceToClosestTile = FVector::Distance(Tile->GetActorLocation(), CloseToLocation);
			ClosestTile = Tile;
		}
	}

	/* Return the the closest board tile if one was found. */
	if (IsValid(ClosestTile))
	{
		return ClosestTile;
	}

	return nullptr;
}
