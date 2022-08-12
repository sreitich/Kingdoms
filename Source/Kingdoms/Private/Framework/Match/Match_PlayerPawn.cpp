// Copyright Samuel Reitich 2022.


#include "Framework/Match/Match_PlayerPawn.h"

#include "Animations/AnimInstance_Parent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Board/BoardTile.h"
#include "Board/TextureBoardPopup.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "Pieces/PieceAIController.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserInterface/MatchSetup/Dragging/PieceDragWidget.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PopUpLocationComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMatch_PlayerPawn::AMatch_PlayerPawn()
{
 	/* Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = true;

	/* Disable ticking by default. */
	SetActorTickEnabled(false);

	/* Pawn should be replicated at all times for all players. */
	bReplicates = true;
	bAlwaysRelevant = true;

	/* Set up this actor's components. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(SceneRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AMatch_PlayerPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AMatch_PlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMatch_PlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMatch_PlayerPawn::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMatch_PlayerPawn::Interact_Released);
}

void AMatch_PlayerPawn::Interact()
{
/* Debugging for piece ID system.
	TArray<AActor*> AllPieces;
	UGameplayStatics::GetAllActorsOfClass(this, AParentPiece::StaticClass(), AllPieces);
	for (AActor* Piece : AllPieces)
	{
		AParentPiece* Pointer = Cast<AParentPiece>(Piece);
		UE_LOG(LogTemp, Error, TEXT("Piece ID: %s"), *Pointer->GetPieceID().ToString());
	}
*/
	/* Stores the location and direction of the player's click. */
	FVector WorldLocation;
	FVector WorldDirection;

	/* Convert the mouse location into a world location. */
	GetController<APlayerController>()->DeprojectMousePositionToWorld(OUT WorldLocation, OUT WorldDirection);

	/* Variables needed for the line trace. */
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;

	/* Line trace from the player 3000 units out to where they clicked. */
	GetWorld()->LineTraceSingleByChannel(OUT HitResult, Camera->GetComponentLocation(), WorldLocation + (WorldDirection * 3000), ECC_Visibility, TraceParams);

	// DrawDebugLine(GetWorld(), Camera->GetComponentLocation(), WorldLocation + (WorldDirection * 3000), FColor(255, 0, 0), true, 20.0f, 0, 5.0f);


	/* If the line trace hit something... */
	if (HitResult.bBlockingHit)
	{
		/* If this pawn's controller's player state is valid... */
		if (GetPlayerState<AMatch_PlayerState>())
		{
			/* Get a copy of this player's player status. REDUNDANT */
			// EPlayerStatus CurrentPlayerStatus = GetPlayerState<AMatch_PlayerState>()->GetCurrentPlayerStatus();

			/* Call corresponding function depending on the state of the player. */
			switch (GetPlayerState<AMatch_PlayerState>()->GetCurrentPlayerStatus())
			{
			/* If the player is placing their pieces... */
			case E_PlacingPieces:
				Interact_PlacingPieces(HitResult);
				break;
			/* If the player is waiting for their turn... */
			case E_WaitingForTurn:
				Interact_WaitingForTurn(HitResult);
				break;
			/* If it's this player's turn and they are selecting a piece... */
			case E_SelectingPiece:
				Interact_SelectingPiece(HitResult);
				break;
			/* If this player is selecting an action from a piece information pop-up... */
			case E_SelectingAction:
				ClearSelection(true);
				break;
			/* If this player is selecting a target for a movement or attack... */
			case E_SelectingTarget_Move:
				Interact_SelectingTargetMove(HitResult);
				break;
			/* If this player is selecting a target for a piece's active ability... */
			case E_SelectingTarget_ActiveAbility:
				Interact_SelectingTargetActiveAbility(HitResult);
				break;
			/* If this player is selecting a target for a piece's passive ability... */
			case E_SelectingTarget_PassiveAbility:
				Interact_SelectingTargetPassiveAbility(HitResult);
				break;
			/* If the player is connecting or placing their pieces, do nothing. This should never be called. */
			default:
				break;
			}
		}
	}
	else
	{
		ClearSelection(true);
	}
}

void AMatch_PlayerPawn::Interact_PlacingPieces(FHitResult InteractionHit)
{
	/* If the player clicked a piece and the piece widget class has been set... */
	if (IsValid(InteractionHit.GetActor()) && Cast<AParentPiece>(InteractionHit.GetActor()) && PieceDragWidgetClass)
	{
		/* Create a new widget to handle the dragging of this piece. */
		PieceDragWidget = CreateWidget<UPieceDragWidget>(Cast<APlayerController>(GetController()), PieceDragWidgetClass, FName("Piece Drag Widget"));

		/* Initialize the widget without spawning its piece, because it's already been spawned. */
		PieceDragWidget->InitializeWidget(false);

		/* Add the widget to the viewport or else the functionality won't work properly.  */
		PieceDragWidget->AddToViewport(0);

		/* Set the widget's spawned piece so it can start dragging it. */
		PieceDragWidget->SpawnedPiece = Cast<AParentPiece>(InteractionHit.GetActor());

		/* Say that the player is currently dragging a piece so the drag stops when the player releases the "interact" key. */
		bIsDraggingPiece = true;
	}
}

void AMatch_PlayerPawn::Interact_WaitingForTurn(FHitResult InteractionHit)
{
	/* Stores which piece to interact with when it's determined whether the player clicked a piece or an occupied tile. */
	AParentPiece* InteractedPiece = nullptr;

	/* If a piece was clicked... */
	if (Cast<AParentPiece>(InteractionHit.Actor))
	{
		/* Interact with the clicked piece. */
		InteractedPiece = Cast<AParentPiece>(InteractionHit.Actor);
	}
	/* If a board tile was clicked... */
	else if (Cast<ABoardTile>(InteractionHit.Actor))
	{
		/* Interact with the piece occupying the clicked tile. */
		InteractedPiece = Cast<ABoardTile>(InteractionHit.Actor)->GetOccupyingPiece();
	}
	/* If anything else was clicked... */
	else
	{
		/* Clear the currently selected piece and remove all piece-information pop-ups. */
		ClearSelection(true);
	}

	/* If the player interacted with a valid piece and it has a valid player controller... */
	if (InteractedPiece && GetController<AMatch_PlayerController>())
	{
		/* If the piece is friendly... */
		if (InteractedPiece->GetInstigator()->IsLocallyControlled())
		{
			/* Select the piece. */
			SelectedPiece = InteractedPiece;

			/* Update and reveal the friendly piece info widget with no buttons. */
			GetController<AMatch_PlayerController>()->UpdatePieceInfoWidget(InteractedPiece, true, false, false);
		}
		/* If the piece is an enemy piece... */
		else
		{
			/* Update and reveal the enemy piece info widget with no buttons. */
			GetController<AMatch_PlayerController>()->UpdatePieceInfoWidget(InteractedPiece, false, false, false);
		}
	}
}

void AMatch_PlayerPawn::Interact_SelectingPiece(FHitResult InteractionHit)
{
	/* Stores which piece to interact with when it's determined whether the player clicked a piece or an occupied tile. */
	AParentPiece* InteractedPiece = nullptr;

	/* If a piece was clicked... */
	if (Cast<AParentPiece>(InteractionHit.Actor))
	{
		/* Interact with the clicked piece. */
		InteractedPiece = Cast<AParentPiece>(InteractionHit.Actor);
	}
	/* If a board tile was clicked. */
	else if (Cast<ABoardTile>(InteractionHit.Actor))
	{
		/* Interact with the piece occupying the clicked tile. */
		InteractedPiece = Cast<ABoardTile>(InteractionHit.Actor)->GetOccupyingPiece();
	}
	/* If anything else was clicked... */
	else
	{
		/* Clear the currently selected piece and remove all piece-information pop-ups. */
		ClearSelection(true);
	}

	/* If the player interacted with a valid piece and the player has a valid player controller... */
	if (IsValid(InteractedPiece) && GetController<AMatch_PlayerController>())
	{
		/* If the piece is friendly... */
		if (InteractedPiece->GetInstigator()->IsLocallyControlled())
		{
			/* Select the piece. */
			SelectedPiece = InteractedPiece;

			/* Update and reveal the friendly piece info widget with buttons. */
			GetController<AMatch_PlayerController>()->UpdatePieceInfoWidget(InteractedPiece, true, true, false);
		}
		/* If the piece is an enemy piece... */
		else
		{
			/* Update and reveal the enemy piece info widget with no buttons. */
			GetController<AMatch_PlayerController>()->UpdatePieceInfoWidget(InteractedPiece, false, false, false);
		}
	}
}

void AMatch_PlayerPawn::Interact_SelectingTargetMove(FHitResult InteractionHit)
{
	/* Stores which tile to interact with when it's determined whether the player clicked a piece or a tile. */
	ABoardTile* InteractedTile = nullptr;

	/* If a piece was clicked... */
	if (AParentPiece* PiecePtr = Cast<AParentPiece>(InteractionHit.Actor))
	{
		/* Interact with the piece's tile. */
		InteractedTile = PiecePtr->GetCurrentTile();
	}
	/* If a board tile was clicked... */
	else if (ABoardTile* TilePtr = Cast<ABoardTile>(InteractionHit.Actor))
	{
		/* Interact with the clicked tile. */
		InteractedTile = TilePtr;
	}
	/* If anything else was clicked... */
	else
	{
		/* Clear the currently selected piece and remove all piece-information pop-ups. */
		ClearSelection(true);
	}

	/* If the player interacted with a tile and the tile is valid... */
	if (IsValid(InteractedTile) && IsValid(SelectedPiece) && SelectedPiece->TileIsInMoveRange(InteractedTile))
	{
		/* If the interacted tile is occupied... */
		if (IsValid(InteractedTile->GetOccupyingPiece()))
		{
			/* If the interacted tile's piece is friendly... */
			if (InteractedTile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled())
			{
				/* Cannot move to occupied piece pop-up. */
			}
			/* If the interacted tile is occupied by an enemy piece... */
			else
			{
				/* If this player has a valid player controller... */
				if (AMatch_PlayerController* ControllerPtr = GetController<AMatch_PlayerController>())
				{
					/* Create and update the attack confirmation pop-up. */
					ControllerPtr->UpdateAttackConfirmationWidget(false, SelectedPiece, InteractedTile->GetOccupyingPiece());

					/* Highlight the pending tile. */
					InteractedTile->Highlight->SetMaterial(0, InteractedTile->Highlight_Target);
				}
			}
		}
		/* If the interacted tile is empty... */
		else
		{
			/* Make sure that this pawn's controller is valid. */
			if (AMatch_PlayerController* ControllerPtr = GetController<AMatch_PlayerController>())
			{
				/* Create and update the move confirmation pop-up. */
				ControllerPtr->UpdateMoveConfirmationWidget(false, InteractedTile, SelectedPiece);

				/* Highlight the pending tile. */
				InteractedTile->Highlight->SetMaterial(0, InteractedTile->Highlight_Target);
			}
		}
	}
	else
	{
		/* Invalid tile pop-up. */
	}
}

void AMatch_PlayerPawn::Interact_SelectingTargetActiveAbility(FHitResult InteractionHit)
{
	/* Stores which piece to interact with when it's determined whether the player clicked a piece or an occupied tile. */
	AParentPiece* InteractedPiece;

	/* If a piece was clicked... */
	if (Cast<AParentPiece>(InteractionHit.Actor))
	{
		/* Interact with the clicked piece. */
		InteractedPiece = Cast<AParentPiece>(InteractionHit.Actor);
	}
	/* If a board tile was clicked... */
	else if (Cast<ABoardTile>(InteractionHit.Actor))
	{
		/* Interact with the piece occupying the clicked tile. */
		InteractedPiece = Cast<ABoardTile>(InteractionHit.Actor)->GetOccupyingPiece();
	}
	/* If anything else was clicked... */
	else
	{
		/* Clear the currently selected piece and remove all piece-information pop-ups. */
		ClearSelection(true);
	}

	/* If the interacted piece is valid for this action... */
	if (true /* call a PieceIsValidActiveAbility function on the selected piece */ )
	{
		/* Confirm action pop-up. */
	}
}

void AMatch_PlayerPawn::Interact_SelectingTargetPassiveAbility(FHitResult InteractionHit)
{
	/* Stores which piece to interact with when it's determined whether the player clicked a piece or an occupied tile. */
	AParentPiece* InteractedPiece;

	/* If a piece was clicked... */
	if (Cast<AParentPiece>(InteractionHit.Actor))
	{
		/* Interact with the clicked piece. */
		InteractedPiece = Cast<AParentPiece>(InteractionHit.Actor);
	}
	/* If a board tile was clicked... */
	else if (Cast<ABoardTile>(InteractionHit.Actor))
	{
		/* Interact with the piece occupying the clicked tile. */
		InteractedPiece = Cast<ABoardTile>(InteractionHit.Actor)->GetOccupyingPiece();
	}
	/* If anything else was clicked... */
	else
	{
		/* Clear the currently selected piece and remove all piece-information pop-ups. */
		ClearSelection(true);
	}

	/* If the interacted piece is valid for this action... */
	if (true /* call a PieceIsValidPassiveAbility function on the selected piece */ )
	{
		/* Confirm action pop-up. */
	}
}

void AMatch_PlayerPawn::Interact_Released()
{
	/* If a piece is currently being dragged... */
	if (bIsDraggingPiece)
	{
		/* Remove the PieceDragWidget widget from its parent, triggering its NativeDestruct function */
		PieceDragWidget->RemoveFromParent();
	}
}

void AMatch_PlayerPawn::ClearSelection(bool bDeselect)
{
	/* If the player has a valid player controller... */
	if (AMatch_PlayerController* PlayerController = Cast<AMatch_PlayerController>(GetController()))
	{
		/* Hide the friendly piece info widget. */
		PlayerController->UpdatePieceInfoWidget(SelectedPiece, true, false, true);

		/* Hide the enemy piece info widget. */
		PlayerController->UpdatePieceInfoWidget(SelectedPiece, false, false, true);

		switch (GetPlayerState<AMatch_PlayerState>()->GetCurrentPlayerStatus())
		{
			/* Remove the move confirmation widget or attack confirmation widget and update the player's state if the
			 * player was targeting a tile to move to. */
			case E_SelectingTarget_Move:
				PlayerController->UpdateMoveConfirmationWidget(true, nullptr, nullptr);
				PlayerController->UpdateAttackConfirmationWidget(true, nullptr, nullptr);
			break;
			/* Remove the active ability confirmation widget. */
			case E_SelectingTarget_ActiveAbility:
				break;
			/* Remove the passive ability confirmation widget. */
			case E_SelectingTarget_PassiveAbility:
				break;
			/* If the player is connecting or placing their pieces, do nothing. This should never be called. */
			default:
				break;
		}
	}

	/* If the selected piece needs to be deselected... */
	if (bDeselect)
	{
		/* SelectedPiece needs to be cleared after the widgets are removed because it's used in updating the piece info. */
		SelectedPiece = nullptr;
	}
}

void AMatch_PlayerPawn::Server_Attack_Implementation(const FAttackInfo InInfo)
{
	Server_Attack_BP(InInfo);
	
	// /* Pass along a copy of the attack information so that it's not lost when the attack function chain is interrupted
	//  * by an animation notify. This is also used to determine which piece is taking damage from the "take damage"
	//  * animation notify. */
	// InInfo.Attacker->SetAttackInfo(InInfo);
	// InInfo.Defender->SetAttackInfo(InInfo);
	//
	// /* If the attacker needs to be next to the defender to attack (e.g. a melee attack) and the two pieces are too far
	//  * apart ("far" is arbitrarily 1 tile), the attacker walks to the defender. */
	// if (InInfo.bMoveTo &&
	// 	(FMath::Abs(InInfo.Attacker->GetCurrentTile()->Coordinates.X - InInfo.Defender->GetCurrentTile()->Coordinates.X) > 2 ||
	// 		FMath::Abs(InInfo.Attacker->GetCurrentTile()->Coordinates.Y - InInfo.Defender->GetCurrentTile()->Coordinates.Y) > 2))
	// {
	// 	/* The attacker walks towards the defender, stopping when it gets within 250 units (about 2 tiles). */
	// 	Cast<APieceAIController>(InInfo.Attacker->GetOwner())->MoveToLocation
	// 	(
	// 		InInfo.Defender->GetActorLocation(),
	// 		250.0f,
	// 		true,
	// 		true,
	// 		false,
	// 		false,
	// 		nullptr
	// 	);
	// }
	//
	// /* Disable input for both players. */
	// for (APlayerState* Player : GetWorld()->GetGameState<AMatch_GameStateBase>()->PlayerArray)
	// {
	// 	if (AMatch_PlayerPawn* PlayerPawn = Cast<AMatch_PlayerPawn>(Player->GetPawn()))
	// 	{
	// 		PlayerPawn->Client_SetUpAttack();
	// 	}
	// }
	//
	// /* Calls Client_InitiateAttack on each client when the attacker and defender are close enough. */
	// WaitForPieceProximity(InInfo);
}

FCameraInterpolationInfo AMatch_PlayerPawn::MovePlayerCameraBP(const AParentPiece* Attacker, const AParentPiece* Defender)
{
	if (IsValid(Attacker) && IsValid(Defender))
	{
		UE_LOG(LogTemp, Error, TEXT("Pieces are valid."));
		
		/* Initialize the vectors we'll need to calculate where to move and rotate the player's camera. */
		FVector StartLoc = SpringArm->GetComponentLocation();
		FRotator StartRot = SpringArm->GetRelativeRotation();
		FVector AttackerLoc = Attacker->GetActorLocation();
		FVector DefenderLoc = Defender->GetActorLocation();

		/* The horizontal distance we want the camera to be from the pieces and the distance we want it to be above them. */
		float HorizontalDistance = FVector::Dist(AttackerLoc, DefenderLoc) * 2.0f;
		float VerticalDistance = 300.0f;

		/* Get the midpoint between the attacker and defender. */
		FVector Midpoint = (AttackerLoc + DefenderLoc) / 2;

		/* Get the normalized difference in the pieces' positions, creating a vector pointing from one piece to the other. */
		FVector NormalizedLocDif = (AttackerLoc - DefenderLoc);
		NormalizedLocDif.Normalize();

		/* Get a normalized vector pointing straight up. */
		FVector WorldUp = FVector(0.0f, 0.0f, 1.0f);

		/* The cross product of the vectors, giving a direction that points perpendicularly away form the axis that connects the pieces. */
		FVector DirectionAwayFromPieces = FVector::CrossProduct(NormalizedLocDif, WorldUp);
		/* Multiply the direction by how far we want the camera to be from the midpoint of the pieces. The camera distance scales linearly from the distance between the pieces. */
		DirectionAwayFromPieces *= HorizontalDistance;
		/* Take the midpoint and move it away and upwards from the pieces, giving us the final location we want our camera to be in. */
		FVector EndLoc = Midpoint + DirectionAwayFromPieces + FVector(0.0f, 0.0f, VerticalDistance);

		/* We want to rotate the camera so that it faces the midpoint of the pieces, and is therefore centered. */
		FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(EndLoc, Midpoint);

		/* Return the information needed to interpolate the camera to the desired transform. */
		FCameraInterpolationInfo CameraInterpolationInfo;
		CameraInterpolationInfo.StartLocation = StartLoc;
		CameraInterpolationInfo.EndLocation = EndLoc;
		CameraInterpolationInfo.StartRotation = StartRot;
		CameraInterpolationInfo.EndRotation = EndRot;
		CameraInterpolationInfo.StartArmLength = SpringArm->TargetArmLength;
		CameraInterpolationInfo.EndArmLength = 0.0f;
		CameraInterpolationInfo.bReverse = false;
		
		return CameraInterpolationInfo;
	}
	
	return FCameraInterpolationInfo();
}

void AMatch_PlayerPawn::Client_MovePlayerCamera_Implementation(const AParentPiece* Attacker,
	const AParentPiece* Defender)
{
	if (IsValid(Attacker) && IsValid(Defender))
	{
		/* Initialize the vectors we'll need to calculate where to move and rotate the player's camera. */
		FVector StartLoc = SpringArm->GetComponentLocation();
		FRotator StartRot = SpringArm->GetRelativeRotation();
		FVector AttackerLoc = Attacker->GetActorLocation();
		FVector DefenderLoc = Defender->GetActorLocation();

		/* The horizontal distance we want the camera to be from the pieces and the distance we want it to be above them. */
		float HorizontalDistance = FVector::Dist(AttackerLoc, DefenderLoc) * 2.0f;
		float VerticalDistance = 300.0f;

		/* Get the midpoint between the attacker and defender. */
		FVector Midpoint = (AttackerLoc + DefenderLoc) / 2;

		/* Get the normalized difference in the pieces' positions, creating a vector pointing from one piece to the other. */
		FVector NormalizedLocDif = (AttackerLoc - DefenderLoc);
		NormalizedLocDif.Normalize();

		/* Get a normalized vector pointing straight up. */
		FVector WorldUp = FVector(0.0f, 0.0f, 1.0f);

		/* The cross product of the vectors, giving a direction that points perpendicularly away form the axis that connects the pieces. */
		FVector DirectionAwayFromPieces = FVector::CrossProduct(NormalizedLocDif, WorldUp);
		/* Multiply the direction by how far we want the camera to be from the midpoint of the pieces. The camera distance scales linearly from the distance between the pieces. */
		DirectionAwayFromPieces *= HorizontalDistance;
		/* Take the midpoint and move it away and upwards from the pieces, giving us the final location we want our camera to be in. */
		FVector EndLoc = Midpoint + DirectionAwayFromPieces + FVector(0.0f, 0.0f, VerticalDistance);

		/* We want to rotate the camera so that it faces the midpoint of the pieces, and is therefore centered. */
		FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(EndLoc, Midpoint);

		/* Smoothly move the camera to or from the given location and rotation. */
		InterpolatePlayerCamera(StartLoc, EndLoc, StartRot, EndRot, SpringArm->TargetArmLength, 0.0f, false);
	}
}

void AMatch_PlayerPawn::Client_SetUpAttack_Implementation()
{
	if (APlayerController* ControllerPtr = Cast<APlayerController>(GetController()))
	{
		/* Disable input for both players. */
		DisableInput(ControllerPtr);
	}
}

void AMatch_PlayerPawn::Client_InitiateAttack_Implementation(FAttackInfo InInfo)
{
	/* Zoom into the fight. */
	MovePlayerCamera(InInfo);

	/* A white highlight glazes over the fighting pieces. */
	InInfo.Defender->FlashHighlight(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), 15.0f, 1.0f);
	InInfo.Attacker->FlashHighlight(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), 15.0f, 1.0f);
	
	if (AMatch_PlayerController* ControllerPtr = Cast<AMatch_PlayerController>(GetController()))
	{
		/* Create an attack graphic and update its display information with these pieces' data. */
		ControllerPtr->UpdateAttackGraphicWidget(false, InInfo.Attacker, InInfo.Defender);
		/* Play the appropriate "fight initiation" animation. */
		ControllerPtr->PlayAttackGraphicAnimation(InInfo.bDefenderFights ? E_NeutralFightInitiation : E_AttackerFightInitiation);
	}

	/* Deprecated: Billboard pop-ups have been removed.
	/* Pop-up an "attacker" indicator over the attacker. #1#
	SpawnTempBillboardPopup(E_AttackerIndicator, InInfo.Attacker->PopUpLocationComponent->GetComponentLocation(), 2.0f);
	/* Pop-up an "attacker" indicator over the defender if they will fight back. Otherwise, use a "defender" indicator. #1#
	SpawnTempBillboardPopup(InInfo.bDefenderFights ? E_AttackerIndicator : E_DefenderIndicator, InInfo.Defender->PopUpLocationComponent->GetComponentLocation(), 2.0f);
	*/
}

void AMatch_PlayerPawn::MovePlayerCamera(FAttackInfo InInfo)
{
	/* Initialize the vectors we'll need to calculate where to move and rotate the player's camera. */
    FVector StartLoc = SpringArm->GetComponentLocation();
	FRotator StartRot = SpringArm->GetRelativeRotation();
    FVector AttackerLoc = InInfo.Attacker->GetActorLocation();
    FVector DefenderLoc = InInfo.Defender->GetActorLocation();

	/* The horizontal distance we want the camera to be from the pieces and the distance we want it to be above them. */
	float HorizontalDistance = FVector::Dist(AttackerLoc, DefenderLoc) * 2.0f;
	float VerticalDistance = 300.0f;

 	/* Get the midpoint between the attacker and defender. */
    FVector Midpoint = (AttackerLoc + DefenderLoc) / 2;

	/* Get the normalized difference in the pieces' positions, creating a vector pointing from one piece to the other. */
	FVector NormalizedLocDif = (AttackerLoc - DefenderLoc);
	NormalizedLocDif.Normalize();

	/* Get a normalized vector pointing straight up. */
	FVector WorldUp = FVector(0.0f, 0.0f, 1.0f);

	/* The cross product of the vectors, giving a direction that points perpendicularly away form the axis that connects the pieces. */
	FVector DirectionAwayFromPieces = FVector::CrossProduct(NormalizedLocDif, WorldUp);
	/* Multiply the direction by how far we want the camera to be from the midpoint of the pieces. The camera distance scales linearly from the distance between the pieces. */
	DirectionAwayFromPieces *= HorizontalDistance;
	/* Take the midpoint and move it away and upwards from the pieces, giving us the final location we want our camera to be in. */
	FVector EndLoc = Midpoint + DirectionAwayFromPieces + FVector(0.0f, 0.0f, VerticalDistance);

	/* We want to rotate the camera so that it faces the midpoint of the pieces, and is therefore centered. */
	FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(EndLoc, Midpoint);

	/* Interpolate the camera to the target location and target rotation. This actually works by interpolating the spring arm to a length of 0, then moving and rotating the spring arm, not just the camera attached to it. */
	InterpolateCamera(StartLoc, EndLoc, StartRot, EndRot, SpringArm->TargetArmLength, 0.0f, false, InInfo);
}

void AMatch_PlayerPawn::SpawnTempBillboardPopup(EAttackBillboardPopUpTexture DisplayedTexture, FVector Location, float Duration) const
{
	/* Define some simple spawn parameters. */
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FRotator Rotation = { 0.0f, 0.0f, 0.0f };
	
	/* Spawn a texture board popup on this client at the given location. */
	ATextureBoardPopup* PopUp = Cast<ATextureBoardPopup>(GetWorld()->SpawnActor(TextureBoardPopupClass,
		&Location, &Rotation, SpawnParams));
	
	/* If the popup actor was successfully spawned... */
	if (IsValid(PopUp))
	{
		/* Get the appropriate texture to display. */
		UTexture2D* DisplayedTexturePtr;
		switch (DisplayedTexture)
		{
		case E_AttackerIndicator:
			DisplayedTexturePtr = PopUp->AttackerIndicator;
			break;
		case E_DefenderIndicator:
			DisplayedTexturePtr = PopUp->DefenderIndicator;
			break;
		case E_DeathIndicator:
			DisplayedTexturePtr = PopUp->DeathIndicator;
			break;
		case E_VictoryIndicator:
			DisplayedTexturePtr = PopUp->VictoryIndicator;
			break;
		case E_DeadlockIndicator:
			DisplayedTexturePtr = PopUp->DeadlockIndicator;
			break;
		default:
			DisplayedTexturePtr = PopUp->AttackerIndicator;
			break;
		}
		
		/* Update the displayed texture and activate the pop-up. */
		PopUp->ActivatePopup(DisplayedTexturePtr, Duration);
	}
}

void AMatch_PlayerPawn::Server_AnimateAttack_Implementation(FAttackInfo InInfo)
{
	/* Only do this once. */
	if (!Cast<AMatch_GameStateBase>(GetWorld()->GetGameState())->bAnimatedPiece)
	{
		Cast<AMatch_GameStateBase>(GetWorld()->GetGameState())->bAnimatedPiece = true;

		/* Trigger the animations everywhere. */
		Multicast_AnimateAttack(InInfo);
	}
}

void AMatch_PlayerPawn::Multicast_AnimateAttack_Implementation(FAttackInfo InInfo)
{
	/* Trigger the attacker animation. */
	Cast<UAnimInstance_Parent>(InInfo.Attacker->GetMesh()->GetAnimInstance())->bAttacking = true;

	/* If the defender fights back, then trigger an attack animation. Otherwise, trigger a "damage-taken" animation. */
	if (InInfo.bDefenderFights)
	{
		Cast<UAnimInstance_Parent>(InInfo.Defender->GetMesh()->GetAnimInstance())->bAttacking = true;
	}
	else
	{
		Cast<UAnimInstance_Parent>(InInfo.Defender->GetMesh()->GetAnimInstance())->bTakingDamage = true;
	}
}

void AMatch_PlayerPawn::Multicast_AnimateDamage_Implementation(FAttackInfo InInfo, bool bAttackerDamaged)
{
	if (bAttackerDamaged)
	{
		if (IsValid(InInfo.Attacker))
			UE_LOG(LogTemp, Error, TEXT("Attacker, %s, took damage!"), *InInfo.Attacker->GetName());
	}
	else
	{
		if (IsValid(InInfo.Defender))
			UE_LOG(LogTemp, Error, TEXT("Defender, %s, took damage!"), *InInfo.Defender->GetName());
	}
}