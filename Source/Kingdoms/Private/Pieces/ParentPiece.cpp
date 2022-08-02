// Copyright Samuel Reitich 2022.


#include "Pieces/ParentPiece.h"

#include "Animations/AnimInstance_Parent.h"
#include "Board/BoardTile.h"
#include "Components/CapsuleComponent.h"
#include "Board/BoardManager.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Pieces/PieceAIController.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Kismet/GameplayStatics.h"

AParentPiece::AParentPiece()
{
 	/* Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = false;

	/* Enable replication. */
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);

	/* Set some class defaults that we want for every piece. */
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	static ConstructorHelpers::FClassFinder<APieceAIController> AIControllerAsset(TEXT("/Game/Blueprints/Pieces/BP_PieceAIController"));
	if (AIControllerAsset.Succeeded())
	{
		AIControllerClass = AIControllerAsset.Class;
	}

	UDataTable* PieceData = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/PieceData.PieceData"));
	if (PieceData)
	{
		PieceDataTable = PieceData;
	}
	
	/* Increase the size of the capsule to fit the entire character mesh. */
	GetCapsuleComponent()->SetCapsuleRadius(44.0f);

	/* Set the mesh's default relative position. */
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	/* Make the mesh invisible. The mesh only acts as a skeleton for the rest of the modular pieces to follow when animating. */
	GetMesh()->SetVisibility(false, false);
	
	/* Set the collision settings for the capsule and mesh components. Check the project's custom collision profiles
	 * for an explanation of each collision preset. */
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PieceCapsule"));
	GetMesh()->SetCollisionProfileName(TEXT("PieceMesh"));
}

void AParentPiece::BeginPlay()
{
	Super::BeginPlay();

	/* Set this piece's skeletal meshes to use a dynamic material instance so that its parameters (like the fresnel) can
	 * be changed during runtime. */
	DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	TArray<USceneComponent*> MeshChildren;
	GetMesh()->GetChildrenComponents(false, MeshChildren);
	for (USceneComponent* SkeletalMesh : MeshChildren)
	{
		Cast<USkeletalMeshComponent>(SkeletalMesh)->SetMaterial(0, DynamicMaterial);
	}
	
	/* Set the animations to use in this piece's animation blueprint. */
	UAnimInstance_Parent* AnimInstance = Cast<UAnimInstance_Parent>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->IdleWalkBS = IdleWalkBS;
		AnimInstance->AttackAnimation = AttackAnimation;
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

		/* Enable this piece's fresnel. */
		DynamicMaterial->SetScalarParameterValue(TEXT("Brightness"), 2.0f);
	}

	/* If the piece data table was found... */
    if (PieceDataTable)
    {
        /* Get this piece's row from the piece data. */
        static const FString ContextString(TEXT("Piece Data Struct"));
        FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

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
}

void AParentPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

void AParentPiece::FlashHighlight(FLinearColor Color, float Brightness, float Duration)
{
	/* Store the original color and brightness to restore after the flash. */
	FLinearColor OriginalColor;
	DynamicMaterial->GetVectorParameterValue(TEXT("FresnelColor"), OriginalColor);
	float OriginalBrightness;
	DynamicMaterial->GetScalarParameterValue(TEXT("Brightness"), OriginalBrightness);

	/* Interpolates to the target color and brightness at the normal speed. Waits for the given duration,
	 * then interpolates back to the original color and brightness. */
	FlashHighlightTimeline(Color, Brightness, OriginalColor, OriginalBrightness, 0.25f,
		Duration);
}

bool AParentPiece::SetCurrentTile(ABoardTile* NewTile)
{
	/* Make sure that the new tile isn't null and that the server is calling this. */
	if (IsValid(NewTile) && HasAuthority())
	{
		/* Change the occupying piece. Only the server can do this. */
		CurrentTile = NewTile;

		return true;
	}
	
	return false;
}

bool AParentPiece::SetCurrentStrength(int NewStrength)
{
	/* Make sure that the server is calling this. */
	if (HasAuthority())
	{
		/* Strength cannot go below 0 or above 20. */
		CurrentStrength = FMath::Clamp(NewStrength, 0, 20);

		return true;
	}

	return false;
}

bool AParentPiece::SetCurrentArmor(int NewArmor)
{
	/* Make sure that the server is calling this. */
	if (HasAuthority())
	{
		/* Armor cannot go below 0 or above 20. */
		CurrentArmor = FMath::Clamp(NewArmor, 0, 20);

		return true;
	}

	return false;
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

TArray<class ABoardTile*> AParentPiece::GetValidTiles()
{
	/* This array of valid tiles is going to be returned. */
	TArray<ABoardTile*> ValidTiles;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the tile's coordinates match with one of this piece's move patterns... */
		if (TileIsInMoveRange(Tile))
		{
			/* Add the tile to the array of valid tiles. */
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
		int NewX = Tile->Coordinates.X, NewY = Tile->Coordinates.Y;
		int OldX = CurrentTile->Coordinates.X, OldY = CurrentTile->Coordinates.Y;
	
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