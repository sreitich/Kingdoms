// Copyright Samuel Reitich 2022.


#include "Framework/Match/Match_PlayerPawn.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "Pieces/PieceAIController.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserInterface/MatchSetup/Dragging/PieceDragWidget.h"

#include "Camera/CameraComponent.h"
#include "Components/PieceNetworkingComponent.h"
#include "GameFramework/GameStateBase.h"
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

	PieceNetworkingComponent = CreateDefaultSubobject<UPieceNetworkingComponent>(TEXT("Piece Networking Component"));
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

	/* If the line trace hit something... */
	if (HitResult.bBlockingHit)
	{
		/* If this pawn's controller's player state is valid... */
		if (GetPlayerState<AMatch_PlayerState>())
		{
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
				Interact_SelectingPiece(HitResult);
				break;
			/* If this player is selecting a target for a movement or attack... */
			case E_SelectingTarget_Move:
				Interact_SelectingTargetMove(HitResult);
				break;
			/* If this player is selecting a target for a piece's active ability... */
			case E_SelectingTarget_ActiveAbility:
				Interact_SelectingTargetActiveAbility(HitResult);
				break;
			/* If the player is connecting or placing their pieces, do nothing. This should never be called. */
			default:
				break;
			}
		}
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
		/* Reset the currently selected actors and remove all piece-information pop-ups. */
		ClearSelection(true, true, true, true, true);
	}

	/* If the player interacted with a valid piece and it has a valid player controller... */
	if (IsValid(InteractedPiece) && GetController<AMatch_PlayerController>())
	{
		/* Get this piece's alignment. */
		const EAlignment Alignment = InteractedPiece->GetLocalAlignment();

		/* Select the piece if it's friendly. */
		if (Alignment == E_Friendly)
		{
			/* Remove the selection reticle from the old piece and reset its fresnel brightness if the player already had one selected. */
			if (IsValid(SelectedPiece))
			{
				SelectedPiece->GetCurrentTile()->bReticleControlledByCursor = true;
				SelectedPiece->GetCurrentTile()->UpdateReticle(false, true);

				SelectedPiece->FlashHighlight(SelectedPiece->FriendlyFresnelColor, SelectedPiece->DefaultFresnelStrength, 1.0f, 0.0f, true);
			}
			
			/* Select the piece. */
			SelectedPiece = InteractedPiece;

			/* Place a green reticle over the selected tile and don't remove it if the player hovers off of it. */
			SelectedPiece->GetCurrentTile()->bReticleControlledByCursor = false;
			SelectedPiece->GetCurrentTile()->UpdateReticle(true, false);

			/* Strengthen the new piece's fresnel to indicate that it is currently selected. */
			SelectedPiece->FlashHighlight(SelectedPiece->FriendlyFresnelColor, SelectedPiece->SelectedFresnelStrength, 1.0f, 0.0f, true);
		}
		else if (Alignment == E_Hostile)
		{
			/* If the player already had an enemy piece selected, reset its fresnel. */
			if (IsValid(SelectedEnemyPiece))
				SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->DefaultFresnelStrength, 1.0, 0.0f, true);

			/* Update the selected enemy piece. */
			SelectedEnemyPiece = InteractedPiece;

			/* Strengthen the enemy piece's fresnel to indicate that it is currently selected. */
			SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->SelectedFresnelStrength, 1.0f, 0.0f, true);
		}

		/* Update and reveal the corresponding piece info widget with no buttons. */
		GetController<AMatch_PlayerController>()->UpdatePieceInfoWidget(InteractedPiece, Alignment, false);
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
		/* Reset the currently selected actors and remove all piece-information pop-ups. */
		ClearSelection(true, true, true, true, true);
	}

	/* If the player interacted with a valid piece and the player has a valid player controller... */
	if (IsValid(InteractedPiece) && GetController<AMatch_PlayerController>())
	{
		/* Get this piece's alignment. */
		const EAlignment Alignment = InteractedPiece->GetLocalAlignment();
		
		/* If the piece is friendly... */
		if (Alignment == E_Friendly)
		{
			/* If a piece has not been selected yet, update the player's status to be selecting an action within a piece info widget. */
		    if (SelectedPiece == nullptr)
		    {
			    Cast<AMatch_PlayerState>(GetPlayerState())->SetPlayerStatus(E_SelectingAction);
		    }
			/* If the selected piece is changing, remove the selection reticle from the previous piece and reset its fresnel brightness. */
		    else
		    {
			    SelectedPiece->GetCurrentTile()->bReticleControlledByCursor = true;
		    	SelectedPiece->GetCurrentTile()->UpdateReticle(false, true);

		    	SelectedPiece->FlashHighlight(SelectedPiece->FriendlyFresnelColor, SelectedPiece->DefaultFresnelStrength, 1.0f, 0.0f, true);
		    }

			/* Select the piece. */
			SelectedPiece = InteractedPiece;

			/* Place a green reticle over the selected tile and don't remove it if the player hovers off of it. */
			SelectedPiece->GetCurrentTile()->bReticleControlledByCursor = false;
			SelectedPiece->GetCurrentTile()->UpdateReticle(true, false);

			/* Strengthen this piece's fresnel to indicate that it is currently selected. */
			SelectedPiece->FlashHighlight(SelectedPiece->FriendlyFresnelColor, SelectedPiece->SelectedFresnelStrength, 1.0f, 0.0f, true);
		}
		/* If the piece is an enemy piece... */
		else if (Alignment == E_Hostile)
		{
			/* If the player already had an enemy piece selected, reset its fresnel. */
			if (IsValid(SelectedEnemyPiece))
				SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->DefaultFresnelStrength, 1.0, 0.0f, true);

			SelectedEnemyPiece = InteractedPiece;

			/* Strengthen the enemy piece's fresnel to indicate that it is currently selected. */
			SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->SelectedFresnelStrength, 1.0f, 0.0f, true);
		}

		/* Update and reveal the corresponding piece info widget. Enable buttons if the selected piece is friendly so that they can perform actions. */
		GetController<AMatch_PlayerController>()->UpdatePieceInfoWidget(InteractedPiece, Alignment, false);
	}
}

void AMatch_PlayerPawn::Interact_SelectingTargetMove(FHitResult InteractionHit)
{
	/* Stores which tile to interact with when it's determined whether the player clicked a piece or a tile. */
	ABoardTile* InteractedTile = nullptr;

	/* If a piece was clicked... */
	if (const AParentPiece* PiecePtr = Cast<AParentPiece>(InteractionHit.Actor))
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
		/* Do nothing. */
		
		/* Reset the currently selected actors and remove all piece-information pop-ups. */
		// ClearSelection(true, true, true, true, true);
	}

	/* If the player interacted with a tile and the tile is valid... */
	if (IsValid(InteractedTile) && IsValid(SelectedPiece) && SelectedPiece->TileIsInMoveRange(InteractedTile))
	{
		/* If the interacted tile is occupied... */
		if (IsValid(InteractedTile->GetOccupyingPiece()))
		{
			/* If the interacted tile's piece is friendly... */
			if (InteractedTile->GetOccupyingPiece()->GetLocalAlignment() == E_Friendly)
			{
				/* Cannot move to occupied piece pop-up. */
			}
			/* If the interacted tile is occupied by an enemy piece... */
			else if (InteractedTile->GetOccupyingPiece()->GetLocalAlignment() == E_Hostile)
			{
				/* If this player has a valid player controller... */
				if (AMatch_PlayerController* ControllerPtr = GetController<AMatch_PlayerController>())
				{
					/* Create and update the attack confirmation pop-up. */
					ControllerPtr->CreateAttackConfirmationWidget(false, SelectedPiece);
					ControllerPtr->UpdateAttackConfirmationWidget(SelectedPiece, InteractedTile->GetOccupyingPiece());

					/* If the player already had a tile selected, remove that tile's selection reticle. */
					if (IsValid(SelectedTile))
					{
						SelectedTile->bReticleControlledByCursor = true;
						SelectedTile->UpdateReticle(false, true);
					}

					/* Save the new tile that the player currently has selected. */
					SelectedTile = InteractedTile;

					/* Place a persistent selection reticle over the newly selected tile. */
					SelectedTile->bReticleControlledByCursor = false;
					SelectedTile->UpdateReticle(true, false);

					/* If the player already had an enemy piece selected, reset its fresnel. */
					if (IsValid(SelectedEnemyPiece))
						SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->DefaultFresnelStrength, 1.0, 0.0f, true);

					/* Update the selected enemy piece. */
					SelectedEnemyPiece = SelectedTile->GetOccupyingPiece();

					/* Strengthen the enemy piece's fresnel to indicate that it is currently selected. */
					SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->SelectedFresnelStrength, 1.0f, 0.0f, true);
				}
			}
		}
		/* If the interacted tile is empty... */
		else
		{
			/* Make sure that this pawn's controller is valid. */
			if (AMatch_PlayerController* ControllerPtr = GetController<AMatch_PlayerController>())
			{
				/* Create a new a move confirmation pop-up if it hasn't been created yet and update it. */
				ControllerPtr->CreateMoveConfirmationWidget(false, SelectedPiece);
				ControllerPtr->UpdateMoveConfirmationWidget(InteractedTile, SelectedPiece);

				/* If the player already had a tile selected, remove that tile's selection reticle. */
				if (IsValid(SelectedTile))
				{
					SelectedTile->bReticleControlledByCursor = true;
					SelectedTile->UpdateReticle(false, true);
				}

				/* Save the new tile that the player currently has selected. */
				SelectedTile = InteractedTile;

				/* Place a persistent selection reticle over the newly selected tile. */
				SelectedTile->bReticleControlledByCursor = false;
				SelectedTile->UpdateReticle(true, false);
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
    /* If the player clicked something... */
    if (IsValid(InteractionHit.GetActor()))
    {
		/* Stores the selected ability target. */
		AActor* SelectedTarget = InteractionHit.GetActor();

		/* Test if the selected target is a valid target. */
		bool bTargetIsValid = false;
		for (const AActor* ValidTarget : SelectedPiece->GetValidActiveAbilityTargets())
		{
			if (ValidTarget == SelectedTarget)
			{
				bTargetIsValid = true;
				break;
			}
		}

		/* If the player selected a valid target for this ability, call a function that creates a confirmation widget
		 * specific to that ability. */
		if (bTargetIsValid)
		{
			/* No abilities currently require the player to select multiple pieces. If this ability had multiple
			 * targets, this function would be skipped over. */
			TArray<AActor*> Targets = TArray<AActor*>();
			Targets.Add(SelectedTarget);

			/* Call the necessary logic for the current piece when a target is selected. This usually just updates the
			 * confirmation widget. */
			SelectedPiece->OnTargetSelected(Targets);

			/* If the player already had as target selected, remove that target's selection reticle. */
			if (IsValid(SelectedTile))
			{
				SelectedTile->bReticleControlledByCursor = true;
				SelectedTile->UpdateReticle(false, true);
			}

			/* Save the new targets that the player currently has selected. */
			if (const AParentPiece* TargetPiece = Cast<AParentPiece>(Targets[0]))
			{
				SelectedTile = TargetPiece->GetCurrentTile();
			}
			else if (ABoardTile* TargetTile = Cast<ABoardTile>(Targets[0]))
			{
				SelectedTile = TargetTile;
			}

			/* Place a persistent selection reticle over the newly selected target. */
			SelectedTile->bReticleControlledByCursor = false;
			SelectedTile->UpdateReticle(true, false);


			/* If the player targeted a piece, strengthen its fresnel. */
			AParentPiece* TargetPiece = nullptr;

			if (AParentPiece* CastTargetPiece = Cast<AParentPiece>(Targets[0]))
			{
				TargetPiece = CastTargetPiece;
			}
			else if (IsValid(SelectedTile->GetOccupyingPiece()))
			{
				TargetPiece = SelectedTile->GetOccupyingPiece();
			}

			/* If the target of a piece's ability is itself, don't mess with its fresnel. */
			if (IsValid(TargetPiece) && TargetPiece != SelectedPiece)
			{
				/* If the player already had a target piece selected, reset its fresnel. */
				if (IsValid(SelectedTargetPiece))
					SelectedTargetPiece->FlashHighlight(SelectedTargetPiece->GetLocalAlignment() == E_Friendly ? SelectedTargetPiece->FriendlyFresnelColor : SelectedTargetPiece->EnemyFresnelColor, SelectedPiece->DefaultFresnelStrength, 1.0, 0.0f, true);

				/* Update the selected target piece. */
				SelectedTargetPiece = TargetPiece;

				/* Strengthen the enemy piece's fresnel to indicate that it is currently selected. */
				SelectedTargetPiece->FlashHighlight(SelectedTargetPiece->GetLocalAlignment() == E_Friendly ? SelectedTargetPiece->FriendlyFresnelColor : SelectedTargetPiece->EnemyFresnelColor, SelectedPiece->SelectedFresnelStrength, 1.0f, 0.0f, true);
			}
		}
		else
		{
			/* "select a valid target" pop-up */
		}
	}
	/* If the player (somehow) didn't click anything. */
	else
	{
		/* "select a valid target" pop-up */
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

void AMatch_PlayerPawn::ClearSelection(bool bPiece, bool bEnemyPiece, bool bTargetPiece, bool bTile, bool bHidePieceInfoWidgets)
{
	/* If the piece info widgets should be hidden... */
	if (bHidePieceInfoWidgets)
	{
		/* If the player has a valid player controller... */
		if (AMatch_PlayerController* PlayerController = Cast<AMatch_PlayerController>(GetController()))
		{
			/* Hide the friendly piece info widget. */
			PlayerController->UpdatePieceInfoWidget(SelectedPiece, E_Friendly, true);

			/* Hide the enemy piece info widget. */
			PlayerController->UpdatePieceInfoWidget(SelectedPiece, E_Hostile, true);
		}
	}

	/* If the selected piece needs to be deselected... */
	if (bPiece && IsValid(SelectedPiece))
	{
		/* Reset the piece's selection reticle. */
		SelectedPiece->GetCurrentTile()->bReticleControlledByCursor = true;
		SelectedPiece->GetCurrentTile()->UpdateReticle(false, true);
		/* Reset the piece's fresnel. */
		SelectedPiece->FlashHighlight(SelectedPiece->FriendlyFresnelColor, SelectedPiece->DefaultFresnelStrength, 1.0f, 0.0f, true);
		/* Clear the pointer to the selected piece. */
		SelectedPiece = nullptr;
	}

	/* If the selected enemy piece needs to be deselected... */
	if (bEnemyPiece && IsValid(SelectedEnemyPiece))
	{
		/* Reset the enemy piece's selection reticle. */
		SelectedEnemyPiece->GetCurrentTile()->bReticleControlledByCursor = true;
		SelectedEnemyPiece->GetCurrentTile()->UpdateReticle(false, true);
		/* Reset the enemy piece's fresnel. */
		SelectedEnemyPiece->FlashHighlight(SelectedEnemyPiece->EnemyFresnelColor, SelectedEnemyPiece->DefaultFresnelStrength, 1.0, 0.0f, true);
		/* Clear the pointer to the selected enemy piece. */
		SelectedEnemyPiece = nullptr;
	}

	/* If the selected target piece needs to be deselected... */
	if (bTargetPiece && IsValid(SelectedTargetPiece))
	{
		/* Reset the target piece's selection reticle. */
		SelectedTargetPiece->GetCurrentTile()->bReticleControlledByCursor = true;
		SelectedTargetPiece->GetCurrentTile()->UpdateReticle(false, true);
		/* Reset the target piece's fresnel. */
		SelectedTargetPiece->FlashHighlight(SelectedTargetPiece->GetLocalAlignment() == E_Friendly ? SelectedTargetPiece->FriendlyFresnelColor : SelectedTargetPiece->EnemyFresnelColor, SelectedTargetPiece->DefaultFresnelStrength, 1.0, 0.0f, true);
		/* Clear the pointer to the target piece. */
		SelectedTargetPiece = nullptr;
	}

	/* If the selected tile needs to be deselected... */
	if (bTile && IsValid(SelectedTile))
	{
		/* Reset the tile's selection reticle. */
		SelectedTile->bReticleControlledByCursor = true;
		SelectedTile->UpdateReticle(false, true);
		/* Clear the pointer to the selected tile. */
		SelectedTile = nullptr;
	}
}

void AMatch_PlayerPawn::Multicast_FlashHighlight_Implementation(AParentPiece* PieceToHighlight, FLinearColor Color,
	float Brightness, float PlayRate, float Duration, bool bIndefiniteDuration)
{
	/* Call the FlashHighlight function with the given piece on each client. */
	PieceToHighlight->FlashHighlight(Color, Brightness, PlayRate, Duration, bIndefiniteDuration);
}

void AMatch_PlayerPawn::Client_DeselectPieceOrTile_Implementation(AParentPiece* PieceToDeselect, ABoardTile* TileToDeselect)
{
	/* Deselect the selected piece if given. */
	if (IsValid(PieceToDeselect) && PieceToDeselect == SelectedPiece)
	{ ClearSelection(true, false, false, false, false); }

	/* Deselect the enemy piece if given. */
	if (IsValid(PieceToDeselect) && PieceToDeselect == SelectedEnemyPiece)
	{ ClearSelection(false, true, false, false, false); }

	/* Deselect the target piece if given. */
	if (IsValid(PieceToDeselect) && PieceToDeselect == SelectedTargetPiece)
	{ ClearSelection(false, false, true, false, false); }

	/* Deselect the tile if given. */
	if (IsValid(TileToDeselect) && TileToDeselect == SelectedTile)
	{ ClearSelection(false, false, false, true, false); }
}

void AMatch_PlayerPawn::Client_RefreshPieceInfoWidgets_Implementation() const
{
	/* Refresh any piece info widget displaying the currently selected friendly or enemy piece. */
	Cast<AMatch_PlayerController>(GetController())->RefreshPieceInfoWidgets(SelectedPiece);
	Cast<AMatch_PlayerController>(GetController())->RefreshPieceInfoWidgets(SelectedEnemyPiece);
}

void AMatch_PlayerPawn::Client_HideWidgetDisplayingPiece_Implementation(AParentPiece* PieceToHide) const
{
	/* Hide any piece info widget currently displaying the given piece. */
	Cast<AMatch_PlayerController>(GetController())->HideWidgetDisplayingPiece(PieceToHide);
}

void AMatch_PlayerPawn::Server_Attack_Implementation(const FAttackInfo InInfo, bool bMoveCamera)
{
	/* Reset the currently selected actors and remove all piece-information pop-ups. */
    ClearSelection(true, true, true, true, true);

	/* Call the blueprint implementation of the attack sequence with server authority. Blueprint-implementable events
	 * can't be RPCs. */
	BP_Attack(InInfo, bMoveCamera);
}

void AMatch_PlayerPawn::Client_MovePlayerCamera_Implementation(const AParentPiece* Attacker,
	const AParentPiece* Defender, bool bReverse)
{
	if (IsValid(Attacker) && IsValid(Defender))
	{
		/* If the camera is being moved to a temporary transform, store the original transform to reverse to afterwards. */
		if (!bReverse)
		{
			OriginalViewTransform.SetLocation(SpringArm->GetComponentLocation());
			OriginalViewTransform.SetRotation(SpringArm->GetComponentRotation().Quaternion());
			OriginalCameraArmLength = SpringArm->TargetArmLength;
		}

		/* Initialize the vectors we'll need to calculate where to move and rotate the player's camera. */
		const FVector AttackerLoc = Attacker->GetActorLocation();
		const FVector DefenderLoc = Defender->GetActorLocation();

		/* The horizontal distance we want the camera to be from the pieces and the distance we want it to be above them. */
		const float HorizontalDistance = FVector::Dist(AttackerLoc, DefenderLoc) * 2.0f;
		const float VerticalDistance = 300.0f;

		/* Get the midpoint between the attacker and defender. */
		const FVector Midpoint = (AttackerLoc + DefenderLoc) / 2;

		/* Get the normalized difference in the pieces' positions, creating a vector pointing from one piece to the other. */
		FVector NormalizedLocDif = (AttackerLoc - DefenderLoc);
		NormalizedLocDif.Normalize();

		/* Get a normalized vector pointing straight up. */
		const FVector WorldUp = FVector(0.0f, 0.0f, 1.0f);

		/* The cross product of the vectors, giving a direction that points perpendicularly away form the axis that connects the pieces. */
		FVector DirectionAwayFromPieces = FVector::CrossProduct(NormalizedLocDif, WorldUp);
		/* Multiply the direction by how far we want the camera to be from the midpoint of the pieces. The camera distance scales linearly from the distance between the pieces. */
		DirectionAwayFromPieces *= HorizontalDistance;
		/* Take the midpoint and move it away and upwards from the pieces, giving us the final location we want our camera to be in. */
		const FVector EndLoc = Midpoint + DirectionAwayFromPieces + FVector(0.0f, 0.0f, VerticalDistance);

		/* We want to rotate the camera so that it faces the midpoint of the pieces, and is therefore centered. */
		const FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(EndLoc, Midpoint);

		/* Smoothly move the camera to or from the given location and rotation. */
		InterpolatePlayerCamera(EndLoc, EndRot, 0.0f, bReverse);
	}
}

void AMatch_PlayerPawn::Server_SetResetAfterMove_Implementation(AParentPiece* PieceToUpdate, bool bNewReset)
{
	/* If the given piece is valid, update its AI's bResetAfterMove variable. */
	if (IsValid(PieceToUpdate))
	{
		Cast<APieceAIController>(PieceToUpdate->GetController())->bResetAfterMove = bNewReset;
	}
}

void AMatch_PlayerPawn::KillPiece_Implementation(AParentPiece* PieceToKill, AActor* Killer, bool bPieceReplaced)
{
	/* Call any piece-specific logic that needs to occur when this piece dies. */
	PieceToKill->OnDeath(Killer);

	/* Remove the killed piece from its owning player's collection of owned pieces. */
	Cast<AMatch_PlayerState>(PieceToKill->GetInstigator()->GetPlayerState())->OwnedPieces.Remove(PieceToKill);

	/* Reset the killed piece's tile if the killed piece was not replaced by the piece that killed it (i.e. as an attack action). */
	if (bPieceReplaced)
	{
		PieceToKill->GetCurrentTile()->SetOccupyingPiece(nullptr);
	}

	/* Refresh each player's piece info widgets, hide any that were displaying the killed piece, and deselect the
	 * killed piece if either player had it selected for any reason. */
	for (const APlayerState* PlayerStatePtr : UGameplayStatics::GetGameState(this)->PlayerArray)
	{
		if (AMatch_PlayerPawn* PlayerPawnPtr = Cast<AMatch_PlayerPawn>(PlayerStatePtr->GetPawn()))
		{
			PlayerPawnPtr->Client_RefreshPieceInfoWidgets();
			PlayerPawnPtr->Client_HideWidgetDisplayingPiece(PieceToKill);
			PlayerPawnPtr->Client_DeselectPieceOrTile(PieceToKill, nullptr);
		}
	}

	/* Destroy this piece. */
	PieceToKill->Destroy();
}

void AMatch_PlayerPawn::Server_UseActiveAbility_Implementation(AParentPiece* AbilityUser, const TArray<AActor*>& Targets)
{
	/* Call the ability with server authority. */
	if (IsValid(AbilityUser))
		AbilityUser->OnActiveAbility(Targets);
}