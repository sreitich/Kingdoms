// Copyright Samuel Reitich 2022.


#include "Pieces/ParentPiece.h"

#include "Animations/AnimInstance_Parent.h"
#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Board/ModifierBoardPopup.h"
#include "Components/CapsuleComponent.h"
#include "Components/PopUpLocationComponent.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pieces/PieceAIController.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AParentPiece::AParentPiece()
{
 	/* Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = true;

	/* Enable replication. */
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);

	/* This determines the location where overhead pop-ups will spawn, and can be changed for each piece. */
	PopUpLocationComponent = CreateDefaultSubobject<UPopUpLocationComponent>(TEXT("Pop-Up Location Component"));
	PopUpLocationComponent->SetupAttachment(GetRootComponent());
	PopUpLocationComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));

	/* Set some class defaults that we want for every piece. */
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = true;

	/* Increase the size of the capsule to fit the entire character mesh. */
	GetCapsuleComponent()->SetCapsuleRadius(44.0f);

	/* Set the mesh's default relative position. */
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	/* Make the mesh invisible. The mesh only acts as a skeleton for the rest of the modular pieces to follow when animating. */
	GetMesh()->SetVisibility(false, false);
	/* Animate every other skeletal mesh even when the parent mesh is invisible. This allows the parent skeletal mesh to
	 * animate everything else while remaining invisible. */
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
    /* Bind these functions to be called when this piece is hovered over. */
    GetMesh()->OnBeginCursorOver.AddDynamic(this, &AParentPiece::OnBeginCursorOver);
    GetMesh()->OnEndCursorOver.AddDynamic(this, &AParentPiece::OnEndCursorOver);

	/* Set the collision settings for the capsule and mesh components. Check the project's custom collision profiles
	 * for an explanation of each collision preset. */
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PieceCapsule"));
	GetMesh()->SetCollisionProfileName(TEXT("PieceMesh"));

	/* Load the piece highlight curve from the content browser. */
	PieceHighlightCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Curves/PieceHighlightCurve.PieceHighlightCurve"));
}

void AParentPiece::BeginPlay()
{
	Super::BeginPlay();

	/* Set this piece's skeletal meshes to use a dynamic material instance so that its parameters (like the fresnel) can
	 * be changed during runtime. */
	DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	TArray<USceneComponent*> MeshChildren;
	GetMesh()->GetChildrenComponents(true, MeshChildren);
	for (USceneComponent* SkeletalMesh : MeshChildren)
	{
		if (USkeletalMeshComponent* SkeletalMeshPtr = Cast<USkeletalMeshComponent>(SkeletalMesh))
			SkeletalMeshPtr->SetMaterial(0, DynamicMaterial);
	}
	
	/* Set the animations to use in this piece's animation blueprint. */
	UAnimInstance_Parent* AnimInstance = Cast<UAnimInstance_Parent>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->IdleWalkBS = IdleWalkBS;
		AnimInstance->AttackAnimation = AttackAnimation;
		AnimInstance->TakingDamageAnimation = TakingDamageAnimation;
		AnimInstance->DeathAnimation = DeathAnimation;
		AnimInstance->CelebrationAnimation = CelebrationAnimation;
		AnimInstance->DeadlockAnimation = DeadlockAnimation;
		AnimInstance->ActiveAbilityAnimation = ActiveAbilityAnimation;
		AnimInstance->PassiveAbilityAnimation = PassiveAbilityAnimation;
		AnimInstance->bActiveAbilityLoops = bActiveAbilityLoops;
	}
	
	/* If the player spawned this piece (during the piece setup phase). */
	if (IsValid(GetInstigator()) && GetInstigator()->IsA(AMatch_PlayerPawn::StaticClass()))
	{
	    /* If the player who spawned this piece is owned by this machine... */
		if (GetInstigator()->IsLocallyControlled())
		{
			/* Don't hide this piece. */
			SetActorHiddenInGame(false);

			/* Set a reference to this pawn's dragged piece. */
			Cast<AMatch_PlayerPawn>(GetInstigator())->DraggedPiece = this;

			/* Give this piece a friendly fresnel. */
			DynamicMaterial->SetVectorParameterValue(TEXT("FresnelColor"), FriendlyFresnelColor);
		}
        /* If this machine wasn't responsible for spawning the piece... , hide it so that players can't see each other's
         * pieces yet. Give the piece an enemy piece fresnel. */
        else
        {
        	/* Hide this piece so that players can't see each other's pieces yet. */
            SetActorHiddenInGame(true);

        	/* Give this piece an enemy fresnel. */
			DynamicMaterial->SetVectorParameterValue(TEXT("FresnelColor"), EnemyFresnelColor);
        }

		/* Initialize the dynamic material's fresnel brightness with the default brightness. */
		DynamicMaterial->SetScalarParameterValue(TEXT("Brightness"), DefaultFresnelStrength);
	}

	/* If the piece data table was found... */
    if (PieceDataTable)
    {
        /* Get this piece's row from the piece data. */
        static const FString ContextString(TEXT("Piece Data Struct"));
        const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

		/* If the data table row was found... */
        if (PieceData)
        {
            /* Set initial stats. */
			CurrentStrength = PieceData->BaseStrength;

			CurrentArmor = PieceData->BaseArmor;

			PassiveCD = 0;

			PassiveUses = PieceData->PassiveUses;

			ActiveCD = 0;

			ActiveUses = PieceData->ActiveUses;
        }
	}

	/* If this piece was spawned during the game, play its pop-up animation. */
	if (AMatch_PlayerPawn* InstigatingPawn = Cast<AMatch_PlayerPawn>(GetInstigator()))
	{
		InstigatingPawn->PlayPiecePopUp(this, 0.25f, false);
	}

	/* Set up the piece highlight timeline. */
	if (PieceHighlightCurve)
	{
		FOnTimelineFloat HighlightTimelineCallback;
		FOnTimelineEventStatic HighlightTimelineFinishedCallback;

		HighlightTimelineCallback.BindUFunction(this, FName("HighlightTimelineTick"));
		HighlightTimelineFinishedCallback.BindUFunction(this, FName("HighlightFadeInEnd"));

		HighlightTimeline.AddInterpFloat(PieceHighlightCurve, HighlightTimelineCallback);
		HighlightTimeline.SetTimelineFinishedFunc(HighlightTimelineFinishedCallback);
	}
}

void AParentPiece::HighlightTimelineTick(float Value)
{
	/* Interpolate the piece's highlight and brightness. */
	DynamicMaterial->SetVectorParameterValue(FName("FresnelColor"), FMath::CInterpTo(OriginalHighlightColor, NewHighlightColor, Value, 1.0f));
	DynamicMaterial->SetScalarParameterValue(FName("Brightness"), FMath::FInterpTo(OriginalHighlightBrightness, NewHighlightBrightness, Value, 1.0f));
}

void AParentPiece::HighlightFadeInEnd()
{
	/* If this highlight has a definite duration and just finished highlighting, wait for the given duration. */
	if (!bIndefiniteHighlightDuration && HighlightTimelineDirection == ETimelineDirection::Forward)
	{
		/* Call HighlightDurationEnd() after the given duration. */
		FTimerHandle HighlightDurationHandle;
		GetWorldTimerManager().SetTimer(HighlightDurationHandle, this, &AParentPiece::HighlightDurationEnd, HighlightDuration, false);
	}
}

void AParentPiece::HighlightDurationEnd()
{
	/* Reverse the highlight to its original color and brightness. */
	HighlightTimelineDirection = ETimelineDirection::Backward;
	HighlightTimeline.ReverseFromEnd();
}

void AParentPiece::OnRep_CurrentStrength()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets(this, false);
}

void AParentPiece::OnRep_CurrentArmor()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets(this, false);
}

void AParentPiece::OnRep_PassiveCooldown()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets(this, false);
}

void AParentPiece::OnRep_PassiveUses()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets(this, false);

}

void AParentPiece::OnRep_ActiveCooldown()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets(this, false);

}

void AParentPiece::OnRep_ActiveUses()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets(this, false);
}

void AParentPiece::OnMoveToTileCompleted()
{
	/* Piece finished moving to a new tile. */
}

void AParentPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HighlightTimeline.TickTimeline(DeltaTime);
}

void AParentPiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /* Replicate these variables. */
    DOREPLIFETIME(AParentPiece, CurrentTile);
	
	DOREPLIFETIME(AParentPiece, CurrentStrength);
	DOREPLIFETIME(AParentPiece, CurrentArmor);
	DOREPLIFETIME(AParentPiece, PassiveCD);
	DOREPLIFETIME(AParentPiece, PassiveUses);
	DOREPLIFETIME(AParentPiece, ActiveCD);
	DOREPLIFETIME(AParentPiece, ActiveUses);

	DOREPLIFETIME(AParentPiece, TemporaryModifiers);
	
	DOREPLIFETIME(AParentPiece, bIsAttacking);
	DOREPLIFETIME(AParentPiece, AttackInfo);
}

void AParentPiece::OnBeginCursorOver(UPrimitiveComponent* Component)
{
	/* If this piece occupies a tile, call that tile's OnBeginCursorOver function, which just handles the logic for
	 * displaying a reticle. */
	if (IsValid(CurrentTile))
		CurrentTile->OnBeginCursorOver(Component);
}

void AParentPiece::OnEndCursorOver(UPrimitiveComponent* Component)
{
	/* If this piece occupies a tile, call that tile's OnEndCursorOver function, which just handles the logic for
	 * removing a displayed reticle. */
	if (IsValid(CurrentTile))
		CurrentTile->OnEndCursorOver(Component);
}

void AParentPiece::Multicast_PlayPiecePopUp_Implementation(float Duration, bool bReverse)
{
	/* Call a pop-up animation with the given parameters for each client. */
	PlayPiecePopUp_BP(Duration, bReverse);
}

void AParentPiece::Server_ResetPieceRotation_Implementation()
{
	/* Get the game state. */
	const AMatch_GameStateBase* GameStatePtr = Cast<AMatch_GameStateBase>(UGameplayStatics::GetGameState(this));
	/* Get the player index of this piece's owning player. */
	const int PlayerIndex = Cast<AMatch_PlayerState>(GetInstigator()->GetPlayerState())->PlayerIndex;
	/* Get the player start that spawned this piece's owning player. */
	const AActor* PlayerStart = GameStatePtr->PlayerStarts[PlayerIndex - 1];
	/* Interpolate this actor's rotation to the rotation that its owning player was spawned at. */
	Cast<AMatch_PlayerPawn>(GetInstigator())->InterpolatePieceRotation(this, nullptr, GetActorRotation(),PlayerStart->GetActorRotation(), false, false);
}

void AParentPiece::FlashHighlight(FLinearColor Color, float Brightness, float PlayRate, float Duration, bool bIndefiniteDuration)
{
	/* Store the original color and brightness to restore after the flash. */
	DynamicMaterial->GetVectorParameterValue(TEXT("FresnelColor"), OriginalHighlightColor);
	DynamicMaterial->GetScalarParameterValue(TEXT("Brightness"), OriginalHighlightBrightness);

	/* Set the other parameters needed for the highlight timeline. */
	NewHighlightColor = Color;
	NewHighlightBrightness = Brightness;
	HighlightPlayRate = PlayRate;
	HighlightDuration = Duration;
	bIndefiniteHighlightDuration = bIndefiniteDuration;

	/* Interpolates to the target color and brightness at the give rate. If bIndefiniteDuration is false, waits for the
	 * given duration, then interpolates back to the original color and brightness. */
	HighlightTimelineDirection = ETimelineDirection::Forward;
	HighlightTimeline.PlayFromStart();
}

void AParentPiece::Multicast_CreateModifierPopUp_Implementation(int ValueChange, bool bStrength)
{
	/* Define additional spawn parameters. */
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	/* Get the location and rotation to spawn this pop-up at. */
	const FVector SpawnLocation = PopUpLocationComponent->GetComponentLocation();
	const FRotator SpawnRotation = PopUpLocationComponent->GetComponentRotation();
	
	/* Spawn a piece of a given class from the server. */
	AModifierBoardPopup* SpawnedPopUp = Cast<AModifierBoardPopup>(GetWorld()->SpawnActor
	(
		ModifierBoardPopupClass,
		&SpawnLocation,
		&SpawnRotation,
		SpawnParams
	));

	/* Activate the pop-up. */
	SpawnedPopUp->BP_ActivateModifierPopUp(SpawnLocation, ValueChange, true, 1.0f);
}

bool AParentPiece::PathToTileIsClear(ABoardTile* TargetTile)
{
	/* Perform a line trace, checking for every tile between the current tile and the target tile within range. */
	TArray<FHitResult> HitResults;
	FCollisionQueryParams TraceParams = FCollisionQueryParams();
	TraceParams.AddIgnoredActors(TArray<AActor*>({GetCurrentTile(), TargetTile}));
	GetWorld()->LineTraceMultiByChannel(OUT HitResults, GetCurrentTile()->GetActorLocation(), TargetTile->GetActorLocation(), ECC_GameTraceChannel1, TraceParams);

	/* Check every tile between the current tile and the target tile. If any of them are occupied by a piece, then the target tile can't be moved to. */
	for (FHitResult HitResult : HitResults)
	{
		if (HitResult.bBlockingHit)
		{
			if (ABoardTile* HitTile = Cast<ABoardTile>(HitResult.GetActor()))
			{
				if (IsValid(HitTile->GetOccupyingPiece()))
				{
					return false;
				}
			}
		}
	}

	return true;
}

void AParentPiece::OnGameStart()
{
	/* Not all pieces require code when the game starts.. */
	// UE_LOG(LogTemp, Error, TEXT("OnGameStart called on a piece that does not implement OnGameStart."));
}

TArray<ABoardTile*> AParentPiece::GetValidMoveTiles()
{
	/* This array of valid tiles is going to be returned. */
	TArray<ABoardTile*> ValidTiles;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the tile's coordinates match with one of this piece's move patterns and the path to the tile is clear, it is a valid destination. */
		if (TileIsInMoveRange(Tile) && PathToTileIsClear(Tile))
		{
			ValidTiles.Add(Tile);
		}
	}

	return ValidTiles;
}

bool AParentPiece::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Store the given tile and current tile's coordinates in variables for readability. */
		const int NewX = Tile->Coordinates.X, NewY = Tile->Coordinates.Y;
		const int OldX = CurrentTile->Coordinates.X, OldY = CurrentTile->Coordinates.Y;
	
		/* Test if the tile's coordinates match with any of this piece's move patterns. */
		if
		(
			/* Forward 1 */
			(NewX == OldX && NewY == OldY + 1) ||
			/* Right 1 */
			(NewX == OldX + 1 && NewY == OldY) ||
			/* Backward 1 */
			(NewX == OldX && NewY == OldY - 1) ||
			/* Left 1 */
			(NewX == OldX - 1 && NewY == OldY) ||
	
			/* Forward 2 */
			(NewX == OldX && NewY == OldY + 2) ||
			/* Right 2 */
			(NewX == OldX + 2 && NewY == OldY) ||
			/* Backward 2 */
			(NewX == OldX && NewY == OldY - 2) ||
			/* Left 2 */
			(NewX == OldX - 2 && NewY == OldY) ||

			/* Forward 3 */
			(NewX == OldX && NewY == OldY + 3) ||
			/* Right 3 */
			(NewX == OldX + 3 && NewY == OldY) ||
			/* Backward 3 */
			(NewX == OldX && NewY == OldY - 3) ||
			/* Left 3 */
			(NewX == OldX - 3 && NewY == OldY)
		)
		{
			return true;
		}
	}
	
	return false;
}

void AParentPiece::OnActiveClicked()
{
	/* By default, highlight every target that the player can select. This can be iterated on if there are abilities that
	* can target things other than pieces or tiles. */
	for (AActor* Target : GetValidActiveAbilityTargets())
	{
		/* If the target is a tile, highlight it, depending on if this ability targets tiles or pieces. */
		if (ABoardTile* Tile = Cast<ABoardTile>(Target))
		{
			/* If the piece data table object was set... */
			if (PieceDataTable)
			{
				/* Get this piece's row from the piece data. */
				static const FString ContextString(TEXT("Piece Data Struct"));
				const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

				/* If the data table row was found... */
				if (PieceData)
				{
					/* If this ability only targets tiles, highlight the target tile yellow. */
					if (PieceData->bActiveTargetsTiles)
					{
						Tile->UpdateEmissiveHighlight(true, 4.0f, Tile->Highlight_ValidUnoccupiedTile);
					}
					/* If this ability targets pieces, highlight the target tile depending on the piece alignment. */
					else if (IsValid(Tile->GetOccupyingPiece()) && !PieceData->bActiveTargetsTiles)
					{
						Tile->UpdateEmissiveHighlight(true, 4.0f, Tile->GetOccupyingPiece()->GetAlignment() == E_Friendly ? Tile->Highlight_Friendly : Tile->Highlight_Enemy);
					}
				}
			}
		}
		/* If the target was a piece, highlight that piece's tile depending on its allegiance. */
		else if (const AParentPiece* Piece = Cast<AParentPiece>(Target))
		{
			ABoardTile* const TargetPieceTile = Piece->GetCurrentTile();
			TargetPieceTile->UpdateEmissiveHighlight(true, 4.0f, Piece->GetAlignment() == E_Friendly ? TargetPieceTile->Highlight_Friendly : TargetPieceTile->Highlight_Enemy);
		}
		/* This can be iterated on if we add pieces that can target things other than tiles or pieces in the future. */
	}
}

void AParentPiece::StartActiveConfirmation(TArray<AActor*> Targets)
{
	UE_LOG(LogTemp, Error, TEXT("StartActiveConfirmation called on a piece without an active ability."));
}

void AParentPiece::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Not all pieces have active abilities. */
	UE_LOG(LogTemp, Error, TEXT("OnActiveAbility called on a piece without an active ability."));
}

void AParentPiece::OnAbilityEffectEnded(TArray<AActor*> Targets)
{
	/* Not all pieces have abilities with effects. */
	// UE_LOG(LogTemp, Error, TEXT("OnAbilityEffectEnded called on a piece that does not implement OnAbilityEffectEnded."));
}

TArray<AActor*> AParentPiece::GetValidActiveAbilityTargets()
{
	/* Not all pieces have active abilities. */
	UE_LOG(LogTemp, Error, TEXT("GetValidActiveAbilityTargets called on a piece without an active ability."));

	/* Return an empty array. */
	return TArray<AActor*>();
}

TArray<AActor*> AParentPiece::GetActiveAbilityRange()
{
	/* Not all pieces have active abilities. */
	UE_LOG(LogTemp, Error, TEXT("GetActiveAbilityRange called on a piece without an active ability."));

	/* Return an empty array. */
	return TArray<AActor*>();
}

void AParentPiece::OnPassiveAbility(TArray<AActor*> Targets)
{
	/* Not all pieces have passive abilities. */
	UE_LOG(LogTemp, Error, TEXT("Passive ability called on a piece without a passive ability."));
}

void AParentPiece::Server_SetCurrentTile_Implementation(ABoardTile* NewTile)
{
	/* Change the occupying piece. Only the server can do this. */
	CurrentTile = NewTile;
}

void AParentPiece::SetAttackInfo(FAttackInfo NewAttackInfo)
{
	/* Only allow the attack info to be updated if it has a valid reference to an attacking piece and a defending piece. */
	if (HasAuthority() && IsValid(NewAttackInfo.Attacker) && IsValid(NewAttackInfo.Defender))
	{
		AttackInfo = NewAttackInfo;
	}
}

void AParentPiece::Server_AddModifier_Implementation(FModifier NewModifier, bool bActivatePopUp, bool bFlashHighlight)
{
	/* Store the original strength or armor value to find out the final change that this modifier applies. */
	const int OriginalValue = NewModifier.EffectedStat ? CurrentStrength : CurrentArmor;
	int NewValue = OriginalValue;

	/* Set the new strength statistic, clamped so that it can't go below 0 or above 20. */
	if (NewModifier.StrengthChange != 0)
	{
		CurrentStrength = FMath::Clamp(CurrentStrength + NewModifier.StrengthChange, 0, 20);
		NewValue = CurrentStrength;
		OnRep_CurrentStrength();
	}

	/* Set the new armor statistic, clamped so that it can't go below 0 or above 20. */
	if (NewModifier.ArmorChange != 0)
	{
		CurrentArmor = FMath::Clamp(CurrentArmor + NewModifier.ArmorChange, 0, 20);
		NewValue = CurrentArmor;
		OnRep_CurrentArmor();
	}

	/* Check if this modifier is already applied. */
	int RepeatIndex = -1;
	for (int i = 0; i < TemporaryModifiers.Num(); i++)
	{
		if (TemporaryModifiers[i].SourceActor == NewModifier.SourceActor &&
			TemporaryModifiers[i].SourceAbilityName == NewModifier.SourceAbilityName &&
			TemporaryModifiers[i].StrengthChange == NewModifier.StrengthChange &&
			TemporaryModifiers[i].ArmorChange == NewModifier.ArmorChange)
		{
			RepeatIndex = i;
			break;
		}
	}

	/* If this modifier is already applied, reset its duration and stack the values together. */
	if (RepeatIndex != -1)
	{
		TemporaryModifiers[RepeatIndex].RemainingDuration = NewModifier.RemainingDuration;
		TemporaryModifiers[RepeatIndex].StrengthChange += NewModifier.StrengthChange;
		TemporaryModifiers[RepeatIndex].ArmorChange += NewModifier.ArmorChange;
	}
	/* If this modifier isn't already applied, apply it. */
	else
	{
		TemporaryModifiers.Add(NewModifier);
	}

	/* Spawn a modifier pop-up if requested. */
	if (bActivatePopUp)
		Multicast_CreateModifierPopUp(NewValue - OriginalValue, NewModifier.EffectedStat == FModifier::Strength);

	/* Flash a piece highlight for each changed statistic on all clients if requested. */
	if (bFlashHighlight)
	{
		if (NewModifier.StrengthChange != 0)
		{
			if (AMatch_PlayerPawn* PawnPtr = Cast<AMatch_PlayerPawn>(GetInstigator()))
			{
				PawnPtr->Multicast_FlashHighlight
				(
					this,
					NewModifier.StrengthChange > 0 ? FLinearColor(0.0f, 1.0f, 0.0f) : FLinearColor(1.0f, 0.0f, 0.0f),
					10.0f,
					0.5f,
					0.25f,
					false
				);
			}
		}

		if (NewModifier.ArmorChange != 0)
		{
			if (AMatch_PlayerPawn* PawnPtr = Cast<AMatch_PlayerPawn>(GetInstigator()))
			{
				PawnPtr->Multicast_FlashHighlight
				(
					this,
					NewModifier.ArmorChange > 0 ? FLinearColor(0.0f, 1.0f, 0.0f) : FLinearColor(1.0f, 0.0f, 0.0f),
					10.0f,
					0.5f,
					0.25f,
					false
				);
			}
		}
	}
}

void AParentPiece::Server_RemoveModifier_Implementation(FModifier ModifierToRemove, bool bActivatePopUp)
{
	/* Some pieces' abilities that have lasting effects (i.e. modifiers) need to execute code when that effect ends. */
	const TArray<AActor*> Targets = { this };
	if (AParentPiece* Piece = Cast<AParentPiece>(ModifierToRemove.SourceActor))
		Piece->OnAbilityEffectEnded(Targets);

	/* Remove the strength effect of the modifier from the piece it was modifying. */
	if (ModifierToRemove.StrengthChange != 0)
	{
		CurrentStrength = FMath::Clamp(CurrentStrength - ModifierToRemove.StrengthChange, 0, 20);
		OnRep_CurrentStrength();
	}

	/* Remove the armor effect of the modifier from the piece it was modifying. */
	if (ModifierToRemove.ArmorChange != 0)
	{
		CurrentArmor = FMath::Clamp(CurrentArmor - ModifierToRemove.ArmorChange, 0, 20);
		OnRep_CurrentArmor();
	}
			
	/* Remove this modifier from this piece. */
	TemporaryModifiers.Remove(ModifierToRemove);
}

void AParentPiece::Server_DecrementModifierDurations_Implementation()
{
	/* For every modifier applied to this piece... */
	for (int i = 0; i < TemporaryModifiers.Num(); i++)
	{
		/* Don't decrement modifiers with an indefinite duration. */
		if (!TemporaryModifiers[i].bIndefiniteDuration)
		{
			/* Reduce the remaining duration by 1 turn. */
			const int NewRemainingDuration = TemporaryModifiers[i].RemainingDuration--;

			/* If the modifier's duration has ended... */
			if (NewRemainingDuration < 1)
			{
				/* Always activate pop-ups for abilities that are removed because their duration ends. */
				Server_RemoveModifier(TemporaryModifiers[i], true);
			}
		}
	}
}

bool AParentPiece::SetPassiveCD(int NewPassiveCD)
{
	/* Make sure that the server is calling this. */
	if (HasAuthority())
	{
		/* Passive cooldown can't be less than 0. */
		if (NewPassiveCD < 0)
		{
			PassiveCD = 0;
		}
		else
		{
			PassiveCD = NewPassiveCD;
		}

		return true;
	}

	return false;
}

bool AParentPiece::SetPassiveUses(int NewPassiveUses)
{
	/* Make sure that the server is calling this. */
	if (HasAuthority())
	{
		/* Passive uses can't be less than 0. */
		if (NewPassiveUses < 0)
		{
			PassiveUses = 0;
		}
		else
		{
			PassiveUses = NewPassiveUses;
		}

		return true;
	}

	return false;
}

bool AParentPiece::SetActiveCD(int NewActiveCD)
{
	/* Make sure that the server is calling this. */
	if (HasAuthority())
	{
		/* Active cooldown can't be less than 0. */
		if (NewActiveCD < 0)
		{
			ActiveCD = 0;
		}
		else
		{
			ActiveCD = NewActiveCD;
		}

		return true;
	}

	return false;
}

bool AParentPiece::SetActiveUses(int NewActiveUses)
{
	/* Make sure that the server is calling this. */
	if (HasAuthority())
	{
		/* Active uses can't be less than 0. */
		if (NewActiveUses < 0)
		{
			ActiveUses = 0;
		}
		else
		{
			ActiveUses = NewActiveUses;
		}

		return true;
	}

	return false;
}