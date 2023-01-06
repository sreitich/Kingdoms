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
#include "Blueprint/UserWidget.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_PlayerController.h"

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

	/* Load the piece pop-up curve from the content browser. */
	PiecePopUpCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Curves/PiecePopUpCurve.PiecePopUpCurve"));

	/* Load the piece rotation curve from the content browser. */
	PieceRotationCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Curves/PieceRotationCurve.PieceRotationCurve"));
	
	/* Load the piece highlight curve from the content browser. */
	PieceHighlightCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Curves/PieceHighlightCurve.PieceHighlightCurve"));
}

void AParentPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* Tick active timelines. */
	PopUpTimeline.TickTimeline(DeltaTime);
	RotationTimeline.TickTimeline(DeltaTime);
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

void AParentPiece::PlayPiecePopUp(float Duration, bool bReverse)
{
	/* Set the necessary rate of the timeline in order for it to be the given duration. 1.0 (the default duration) /
	 * Duration (the desired duration) = the desired rate (distance/time=speed). */
	PopUpTimeline.SetPlayRate(1.0f / Duration);
	PopUpTimeline.PlayFromStart();
}

void AParentPiece::InterpolatePieceRotation(ABoardTile* NewTile, FRotator OriginalRot, FRotator TargetRot,
	bool bMoveWhenFinished, bool bResetStateWhenFinished)
{
	/* Set the parameters needed for the piece rotation timeline. */
	RotationNewTile = NewTile;
	OriginalRotationRot = OriginalRot;
	TargetRotationRot = TargetRot;
	bRotationMoveWhenFinished = bMoveWhenFinished;
	bRotationResetStateWhenFinished = bResetStateWhenFinished;

	/* Play the timeline from the start. */
	RotationTimeline.PlayFromStart();
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
	InterpolatePieceRotation(nullptr, GetActorRotation(),PlayerStart->GetActorRotation(), false, false);
}

void AParentPiece::HighlightTarget()
{
	/* Highlight this piece's tile depending on its alignment. */
	CurrentTile->HighlightTarget();
}

void AParentPiece::RemoveTargetHighlight()
{
	/* Remove the highlight from this piece's tile. */
	CurrentTile->RemoveTargetHighlight();
}

void AParentPiece::FlashHighlight(FLinearColor Color, float Brightness, float PlayRate, float Duration, bool bIndefiniteDuration)
{
	/* Store the original color and brightness to restore after the flash. */
	DynamicMaterial->GetVectorParameterValue(TEXT("FresnelColor"), OriginalHighlightColor);
	DynamicMaterial->GetScalarParameterValue(TEXT("Brightness"), OriginalHighlightBrightness);

	/* Set the other parameters needed for the highlight timeline. */
	TargetHighlightColor = Color;
	TargetHighlightBrightness = Brightness;
	HighlightPlayRate = PlayRate;
	HighlightDuration = Duration;
	bIndefiniteHighlightDuration = bIndefiniteDuration;

	/* Interpolates to the target color and brightness at the give rate. If bIndefiniteDuration is false, waits for the
	 * given duration, then interpolates back to the original color and brightness. */
	HighlightTimelineDirection = ETimelineDirection::Forward;
	HighlightTimeline.PlayFromStart();
}

void AParentPiece::Multicast_CreateModifierPopUp_Implementation(int ValueChange, bool bStrength, bool bFlashHighlight)
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

	/* Activate the pop-up. The pop-up follows this piece. */
	SpawnedPopUp->BP_ActivateModifierPopUp(this, SpawnLocation, ValueChange, bStrength, ModifierPopUpDuration, true);

	/* Flash a modifier highlight if requested. */
	if (bFlashHighlight)
		Cast<AMatch_PlayerPawn>(GetInstigator())->Multicast_FlashHighlight
		(
			this,
			ValueChange > 0 ? BuffColor : DebuffColor,
			FlashFresnelStrength,
			ModifierFlashPlayRate,
			ModifierFlashDuration,
			false
		);
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
		/* If the tile's coordinates match with one of this piece's move patterns, and the path to the tile is clear,
		 * and it is empty or not occupied by a friendly piece, it is a valid destination. */
		if (TileIsInMoveRange(Tile) && PathToTileIsClear(Tile) && (!IsValid(Tile->GetOccupyingPiece()) || Tile->GetOccupyingPiece()->GetLocalAlignment() != E_Friendly))
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
	/* By default, highlight every target that the player can select. This can be iterated on if there are more actors
	 * that can be targeted for abilities, in which case they just need to implement the target interface. */
	for (AActor* Target : GetValidActiveAbilityTargets())
	{
		/* If this target implements the target interface (which all possible target actors should), highlight it. */
		if (Target->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
		{
			Cast<ITargetInterface>(Target)->HighlightTarget();
		}
	}
}

void AParentPiece::OnTargetSelected(TArray<AActor*> Targets)
{
	/* Update this piece's active ability confirmation widget. */
	Piece_UpdateActiveConfirmation(Targets);
}

void AParentPiece::Piece_UpdateActiveConfirmation(TArray<AActor*> Targets)
{
	/* Not all pieces have active abilities. */
	UE_LOG(LogTemp, Error, TEXT("Piece_UpdateActiveConfirmation called on a piece without an active ability."));
}

TArray<AActor*> AParentPiece::GetValidActiveAbilityTargets()
{
	/* Not all pieces have active abilities. */
	UE_LOG(LogTemp, Error, TEXT("GetValidActiveAbilityTargets called on a piece without an active ability."));

	/* Return an empty array. */
	return TArray<AActor*>();
}

TArray<ABoardTile*> AParentPiece::GetActiveAbilityRange()
{
	/* Not all pieces have active abilities. */
	UE_LOG(LogTemp, Error, TEXT("GetActiveAbilityRange called on a piece without an active ability."));

	/* Return an empty array. */
	return TArray<ABoardTile*>();
}

void AParentPiece::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Set that this player is in a sequence, preventing them from taking actions or ending their turn. This is set back
	 * to false in OnActiveAbilityEnded. */
	GetInstigator()->GetPlayerState<AMatch_PlayerState>()->Client_SetIsInSequence(true);

	/* Record that the player used their ability action for this turn, preventing them from using another active ability
	 * until their next turn. */
	Cast<AMatch_PlayerPawn>(GetInstigator())->GetPlayerState<AMatch_PlayerState>()->SetAbilityActionUsed();

	/* Get this piece's piece data to trigger its cooldown and decrement its uses as needed. */
	if (PieceDataTable)
	{
		static const FString ContextString(TEXT("Piece Data Struct"));
		const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

		if (PieceData)
		{
			/* Put the ability onto cooldown if it has one. */
			if (PieceData->ActiveCD > 0)
				SetActiveCD(PieceData->ActiveCD);

			/* Decrement this ability's remaining uses if it has limited uses. */
			if (PieceData->ActiveUses > 0)
				SetActiveUses(ActiveUses - 1);
		}
	}
}

void AParentPiece::OnActiveAbilityEnded()
{
	/* Set that this player is no longer in a sequence, allowing them to take actions or end their turn. */
	GetInstigator()->GetPlayerState<AMatch_PlayerState>()->Client_SetIsInSequence(false);
}

void AParentPiece::OnAbilityEffectEnded(TArray<AActor*> Targets)
{
	/* Not all pieces have abilities with effects. */
	// UE_LOG(LogTemp, Error, TEXT("OnAbilityEffectEnded called on a piece that does not implement OnAbilityEffectEnded."));
}

TArray<AActor*> AParentPiece::GetValidPassiveAbilityTargets()
{
	/* Not all pieces have passive abilities. */
	UE_LOG(LogTemp, Error, TEXT("GetValidPassiveAbilityTargets called on a piece without a passive ability."));

	/* Return an empty array. */
	return TArray<AActor*>();
}

TArray<ABoardTile*> AParentPiece::GetPassiveAbilityRange()
{
	/* Not all pieces have passive abilities. */
	UE_LOG(LogTemp, Error, TEXT("GetPassiveAbilityRange called on a piece without a passive ability."));

	/* Return an empty array. */
	return TArray<ABoardTile*>();
}

void AParentPiece::OnPassiveAbility(TArray<AActor*> Targets)
{
	/* Not all pieces have passive abilities. */
	UE_LOG(LogTemp, Error, TEXT("Passive ability called on a piece without a passive ability."));
}

void AParentPiece::OnDeath(AActor* Killer)
{
	/* No default logic. */
}

void AParentPiece::SetCurrentTile(ABoardTile* NewTile)
{
	/* Locally update this piece's current tile to reduce delay. */
	CurrentTile = NewTile;

	/* If this was called on a non-server client, update this piece's current tile on the server. */
	if (!HasAuthority())
	{
		Server_SetCurrentTile(NewTile);
	}
}

void AParentPiece::SetAttackInfo(FAttackInfo NewAttackInfo)
{
	/* The given attacker and defender must both be valid for the variable to be updated. */
	if (IsValid(NewAttackInfo.Attacker) && IsValid(NewAttackInfo.Defender))
	{
		/* Locally update the current attack's information to reduce delay. */
		AttackInfo = NewAttackInfo;

		/* If this was called on a non-server client, update the current attack's information on the server. */
		if (!HasAuthority())
		{
			Server_SetAttackInfo(NewAttackInfo);
		}
	}
}

void AParentPiece::OnRep_TemporaryModifiers(TArray<FModifier> OldTemporaryModifiers)
{
	/* Apply the effects of the added or removed modifier on the server. */
	if (HasAuthority())
	{
		/* A modifier was added. */
		if (TemporaryModifiers.Num() > OldTemporaryModifiers.Num())
		{
			/* Get a reference to the most recently added modifier. */
			FModifier& NewModifier = TemporaryModifiers.Last(0);

			/* Set the new strength statistic, clamped so that it can't go above 20. */
			if (NewModifier.StrengthChange != 0)
			{
				CurrentStrength = FMath::Clamp(CurrentStrength + NewModifier.StrengthChange, 0, 20);

				/* Create a strength modifier pop-up and highlight if this modifier's strength pop-up hasn't been played and there isn't a pop-up already playing. */
				if (!NewModifier.bStrPopUpPlayed && !bIsModifierPopUpPlaying)
				{
					Multicast_CreateModifierPopUp(NewModifier.StrengthChange, true, true);

					/* Prevent this modifier from playing another strength pop-up. */
					NewModifier.bStrPopUpPlayed = true;
					/* Prevent another pop-up from being played before this one finishes. */
					bIsModifierPopUpPlaying = true;
				}

				/* Manually call the OnRep on the server. */
				OnRep_CurrentStrength();
			}

			/* Set the new armor statistic, clamped so that it can't go above 20. */
			if (NewModifier.ArmorChange != 0)
			{
				CurrentArmor = FMath::Clamp(CurrentArmor + NewModifier.ArmorChange, 0, 20);

				/* Create a armor modifier pop-up and highlight if this modifier's armor pop-up hasn't been played and there isn't a pop-up already playing. */
				if (!NewModifier.bArmPopUpPlayed && !bIsModifierPopUpPlaying)
				{
					Multicast_CreateModifierPopUp(NewModifier.ArmorChange, false, true);

					/* Prevent this modifier from playing another armor pop-up. */
					NewModifier.bArmPopUpPlayed = true;
					/* Prevent another pop-up from being played before this one finishes. */
					bIsModifierPopUpPlaying = true;
				}

				/* Manually call the OnRep on the server. */
				OnRep_CurrentArmor();
			}
		}
		/* A modifier was removed. */
		else if (TemporaryModifiers.Num() < OldTemporaryModifiers.Num())
		{
			/* Get the modifier that was just removed. */
			const FModifier RemovedModifier = OldTemporaryModifiers.Last(0);

			/* Set the new strength statistic, clamped so that it can't go below 0. */
			if (RemovedModifier.StrengthChange != 0)
			{
				CurrentStrength = FMath::Clamp(CurrentStrength - RemovedModifier.StrengthChange, 0, 20);
				/* Manually call the OnRep on the server. */
				OnRep_CurrentStrength();
			}

			/* Set the new armor statistic, clamped so that it can't go below 0. */
			if (RemovedModifier.ArmorChange != 0)
			{
				CurrentArmor = FMath::Clamp(CurrentArmor - RemovedModifier.ArmorChange, 0, 20);
				/* Manually call the OnRep on the server. */
				OnRep_CurrentArmor();
			}
		}
	}
}

void AParentPiece::SetPassiveCD(int NewPassiveCD)
{
	/* Clamp the given passive ability cooldown so that it doesn't go below 0. */
	const int ClampedPassiveCD = FMath::Clamp(NewPassiveCD, 0, NewPassiveCD);

	/* Locally update this piece's passive ability cooldown to reduce delay. */
	PassiveCD = ClampedPassiveCD;

	/* OnRep functions are not called automatically locally. */
	OnRep_PassiveCooldown();

	/* Update this piece's passive ability cooldown on the server to replicate the change to all clients. */
	Server_SetPassiveCD(ClampedPassiveCD);
}

void AParentPiece::SetPassiveUses(int NewPassiveUses)
{
	/* Clamp the given remaining passive ability uses so that they don't go below 0. */
	const int ClampedPassiveUses = FMath::Clamp(NewPassiveUses, 0, NewPassiveUses);

	/* Locally update this piece's remaining passive ability uses to reduce delay. */
	PassiveCD = ClampedPassiveUses;

	/* OnRep functions are not called automatically locally. */
	OnRep_PassiveUses();

	/* Update this piece's remaining passive ability uses on the server to replicate the change to all clients. */
	Server_SetPassiveUses(ClampedPassiveUses);
}

void AParentPiece::SetActiveCD(int NewActiveCD)
{
	/* Clamp the given active ability cooldown so that it doesn't go below 0. */
	const int ClampedActiveCD = FMath::Clamp(NewActiveCD, 0, NewActiveCD);

	/* Locally update this piece's active ability cooldown to reduce delay. */
	ActiveCD = ClampedActiveCD;

	/* OnRep functions are not called automatically locally. */
	OnRep_ActiveCooldown();

	/* Update this piece's active ability cooldown on the server to replicate the change to all clients. */
	Server_SetActiveCD(ClampedActiveCD);
}

void AParentPiece::SetActiveUses(int NewActiveUses)
{
	/* Clamp the given remaining active ability uses so that they don't go below 0. */
	const int ClampedActiveUses = FMath::Clamp(NewActiveUses, 0, NewActiveUses);

	/* Locally update this piece's remaining active ability uses to reduce delay. */
	PassiveCD = ClampedActiveUses;

	/* OnRep functions are not called automatically locally. */
	OnRep_ActiveUses();

	/* Update this piece's remaining active ability uses on the server to replicate the change to all clients. */
	Server_SetActiveUses(ClampedActiveUses);
}

void AParentPiece::BeginPlay()
{
	Super::BeginPlay();

	/* Set this piece's skeletal meshes to use a dynamic material instance so that its parameters (like the fresnel) can
	 * be changed during runtime. */
	DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	GetMesh()->SetMaterial(0, DynamicMaterial);
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
	if (const AMatch_PlayerPawn* InstigatingPawn = Cast<AMatch_PlayerPawn>(GetInstigator()))
	{
		PlayPiecePopUp(0.25f, false);
	}

	/* Set up the piece pop-up timeline. */
	if (PiecePopUpCurve)
	{
		FOnTimelineFloat PopUpTimelineCallback;
		PopUpTimelineCallback.BindUFunction(this, FName("PopUpTimelineTick"));
		PopUpTimeline.AddInterpFloat(PiecePopUpCurve, PopUpTimelineCallback);
	}

	/* Set up the piece rotation timeline. */
	if (PieceRotationCurve)
	{
		FOnTimelineFloat RotationTimelineCallback;
		FOnTimelineEventStatic RotationTimelineFinishedCallback;

		RotationTimelineCallback.BindUFunction(this, FName("RotationTimelineTick"));
		RotationTimelineFinishedCallback.BindUFunction(this, FName("RotationTimelineEnd"));

		RotationTimeline.AddInterpFloat(PieceRotationCurve, RotationTimelineCallback);
		RotationTimeline.SetTimelineFinishedFunc(RotationTimelineFinishedCallback);
	}

	/* Set up the piece highlight timeline. */
	if (PieceHighlightCurve)
	{
		FOnTimelineFloat HighlightTimelineCallback;
		FOnTimelineEventStatic HighlightTimelineFinishedCallback;

		HighlightTimelineCallback.BindUFunction(this, FName("HighlightTimelineTick"));
		HighlightTimelineFinishedCallback.BindUFunction(this, FName("HighlightTimelineEnd"));

		HighlightTimeline.AddInterpFloat(PieceHighlightCurve, HighlightTimelineCallback);
		HighlightTimeline.SetTimelineFinishedFunc(HighlightTimelineFinishedCallback);
	}
}

void AParentPiece::PopUpTimelineTick(float Value)
{
	/* Set the piece's size to the current value of the timeline. */
	GetMesh()->SetRelativeScale3D(FVector(Value));
}

void AParentPiece::RotationTimelineTick(float Value)
{
	/* Rotate the actor towards the target rotation. */
	SetActorRotation(FMath::RInterpTo(OriginalRotationRot, TargetRotationRot, Value, 1.0f));

	/* If the piece is rotating in order to move to a new location, start moving to the new tile when the piece is
	 * halfway through its rotation. This just gives the movement a smoother feel. */
	if (bRotationMoveWhenFinished && Value > 0.5 && !bRotationStartedMoving)
	{
		/* Only move the piece once. */
		bRotationStartedMoving = true;

		Cast<AMatch_PlayerController>(Cast<AMatch_PlayerPawn>(GetInstigator())->GetController())->GetServerCommunicationComponent()->MovePieceToTile
		(
			this,
			RotationNewTile,
			bRotationResetStateWhenFinished
		);
	}
}

void AParentPiece::RotationTimelineEnd()
{
	bRotationStartedMoving = false;
}

void AParentPiece::HighlightTimelineTick(float Value)
{
	/* Interpolate the piece's highlight and brightness. */
	DynamicMaterial->SetVectorParameterValue(FName("FresnelColor"), FMath::CInterpTo(OriginalHighlightColor, TargetHighlightColor, Value, 1.0f));
	DynamicMaterial->SetScalarParameterValue(FName("Brightness"), FMath::FInterpTo(OriginalHighlightBrightness, TargetHighlightBrightness, Value, 1.0f));
}

void AParentPiece::HighlightTimelineEnd()
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
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets();
}

void AParentPiece::OnRep_CurrentArmor()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets();
}

void AParentPiece::OnRep_PassiveCooldown()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets();
}

void AParentPiece::OnRep_PassiveUses()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets();

}

void AParentPiece::OnRep_ActiveCooldown()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets();

}

void AParentPiece::OnRep_ActiveUses()
{
	/* Refresh any piece info widgets currently displaying this piece's info. */
	Cast<AMatch_PlayerPawn>(UGameplayStatics::GetPlayerPawn(this, 0))->Client_RefreshPieceInfoWidgets();
}

void AParentPiece::OnMoveToTileCompleted()
{
	/* Piece finished moving to a new tile. */
}

void AParentPiece::Server_SetCurrentTile_Implementation(ABoardTile* NewTile)
{
	/* Set this piece's current tile on the server. */
	CurrentTile = NewTile;
}

void AParentPiece::Server_SetAttackInfo_Implementation(FAttackInfo NewAttackInfo)
{
	/* Set the information for the current attack on the server. */
	AttackInfo = NewAttackInfo;
}

void AParentPiece::Server_SetPassiveCD(int NewPassiveCD)
{
	/* Set this piece's passive ability cooldown on the server. */
	PassiveCD = NewPassiveCD;

	/* OnRep functions are not called automatically locally. */
	OnRep_PassiveCooldown();
}

void AParentPiece::Server_SetPassiveUses(int NewPassiveUses)
{
	/* Set this piece's remaining passive ability uses on the server. */
	PassiveUses = NewPassiveUses;

	/* OnRep functions are not called automatically locally. */
	OnRep_PassiveUses();
}

void AParentPiece::Server_SetActiveCD(int NewActiveCD)
{
	/* Set this piece's active ability cooldown on the server. */
	ActiveCD = NewActiveCD;

	/* OnRep functions are not called automatically locally. */
	OnRep_ActiveCooldown();
}

void AParentPiece::Server_SetActiveUses(int NewActiveUses)
{
	/* Set this piece's remaining active ability uses on the server. */
	ActiveUses = NewActiveUses;

	/* OnRep functions are not called automatically locally. */
	OnRep_ActiveUses();
}