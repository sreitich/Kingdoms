// Copyright Change Studios, LLC 2023.


#include "Board/BoardTile.h"

#include "Board/BoardManager.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Pieces/ParentPiece.h"

ABoardTile::ABoardTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* Enable replication. */
	bReplicates = true;

	/* Set up all of this actor's components. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(SceneRoot);

	/* Bind these functions to be called when this tile is hovered over. */
	Body->OnBeginCursorOver.AddDynamic(this, &ABoardTile::OnBeginCursorOver);
	Body->OnEndCursorOver.AddDynamic(this, &ABoardTile::OnEndCursorOver);

	Checker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Checker"));
	Checker->SetupAttachment(Body);
	Checker->SetIsReplicated(false);

	Highlight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Highlight"));
	Highlight->SetupAttachment(Body);
	Highlight->SetIsReplicated(false);

	Reticle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Reticle"));
	Reticle->SetupAttachment(Body);
	Reticle->SetIsReplicated(false);

	EmissiveHighlight = CreateDefaultSubobject<URectLightComponent>(TEXT("Emissive Highlight"));
	EmissiveHighlight->SetupAttachment(Body);
	EmissiveHighlight->SetIsReplicated(false);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Body);
}

bool ABoardTile::operator<(const ABoardTile& Other) const
{
	/* If a tile has a lesser Y-coordinate, then it will always be lesser. */
	if (Coordinates.Y < Other.Coordinates.Y)
	{
		return true;
	}
	/* If a tile has a greater Y-coordinate, then it will never be lesser. */
	else if (Coordinates.Y > Other.Coordinates.Y)
	{
		return false;
	}
	/* If a tile has the same Y-coordinates, then their difference is determined by their X-coordinates. */
	else
	{
		return Coordinates.X < Other.Coordinates.X;
	}
}

bool ABoardTile::operator==(const ABoardTile& Other) const
{
	/* If the coordinates are identical, then the tiles are the same. */
	if (Coordinates.X == Other.Coordinates.X && Coordinates.Y == Other.Coordinates.Y)
		return true;

	return false;
}

void ABoardTile::BeginPlay()
{
	Super::BeginPlay();

	/* Create a dynamic material for the reticle mesh, allowing the material's parameters to be changed during runtime. */
	ReticleMaterial = UMaterialInstanceDynamic::Create(Reticle->GetMaterial(0), this);
	Reticle->SetMaterial(0, ReticleMaterial);
}

void ABoardTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoardTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /* Replicate these variables. */
    DOREPLIFETIME(ABoardTile, OccupyingPiece);
}

void ABoardTile::OnBeginCursorOver(UPrimitiveComponent* Component)
{
	/* Only update this tile's reticle if it is being controlled by cursor events and if the player's input is enabled. */
	if (bReticleControlledByCursor && IsValid(UGameplayStatics::GetPlayerPawn(this, 0)) && UGameplayStatics::GetPlayerPawn(this, 0)->InputEnabled())
	{
		if (const AMatch_PlayerState* LocalPlayerState = Cast<AMatch_PlayerState>(UGameplayStatics::GetPlayerPawn(this, 0)->GetPlayerState()))
		{
			switch (LocalPlayerState->GetCurrentPlayerStatus())
			{

			/* If the player is selecting a piece, display a reticle on the tile that they are currently hovering over. */
			case E_SelectingPiece:
				UpdateReticle(true, true);
				break;

			/* If the player is waiting for their turn, display a reticle on the tile that they are currently hovering over,
			 * unless the tile is occupied by their currently selected piece, if they have one. */
			case E_WaitingForTurn:
				UpdateReticle(true, true);
				break;

			/* When a piece is currently selected, display a reticle over the tile that the player is currently hovering
			 * over, unless the tile is occupied by the selected piece. */
			case E_SelectingAction:
				UpdateReticle(true, true);
				break;

			/* When the player is selecting a movement destination, display a green reticle when hovering over a valid
			 * destination and a yellow reticle over all other tiles. */
			case E_SelectingTarget_Move:
				/* This tile is only valid if its emissive highlight is currently visible. */
				UpdateReticle(true, !EmissiveHighlight->IsVisible());

			/* When a player is selecting a target for an active ability, display a green reticle when hovering over a
			 * valid target tile or piece and a yellow reticle over all other tiles. */
			case E_SelectingTarget_ActiveAbility:
				/* This tile is only valid if its emissive highlight is currently visible. */
				UpdateReticle(true, !EmissiveHighlight->IsVisible());

			/* If the player is in any other state, don't display a reticle. */
			default:
				break;
			}
		}
	}
}

void ABoardTile::OnEndCursorOver(UPrimitiveComponent* Component)
{
	/* Allows us to control when this reticle is removed, if ever. */
	if (bReticleControlledByCursor)
	{
		UpdateReticle(false, true);
	}
}

void ABoardTile::HighlightTarget(bool bTargetedByFriendly)
{
	/* If this tile is occupied, highlight it relative to the alignment of the piece targeting it and depending on the
	 * occupying piece's alignment. */
	if (IsValid(OccupyingPiece))
	{
		/* If this tile is being targeted by a friendly piece, then highlight the tile relative to the local player. */
		if (bTargetedByFriendly)
			UpdateEmissiveHighlight(true, DefaultHighlightPlayRate, GetOccupyingPiece()->GetLocalAlignment() == E_Friendly ? Highlight_Friendly : Highlight_Enemy);
		/* If this tile is being targeted by an enemy piece, then highlight the tile relative to the local player's
		 * opponent. */
		else
			UpdateEmissiveHighlight(true, DefaultHighlightPlayRate, GetOccupyingPiece()->GetLocalAlignment() == E_Hostile ? Highlight_Friendly : Highlight_Enemy);
	}
	/* If this tile is empty, highlight it as unoccupied. */
	else
	{
		UpdateEmissiveHighlight(true, DefaultHighlightPlayRate, Highlight_ValidUnoccupiedTile);
	}
}

void ABoardTile::RemoveTargetHighlight()
{
	/* Hide this tile's highlight. */
	UpdateEmissiveHighlight(false, DefaultHighlightPlayRate, EmissiveHighlight->GetLightColor());
}

void ABoardTile::UpdateReticle(bool bReveal, bool bYellow)
{
	/* Exception catch: some crashes were being caused by these not being initialized yet. */
	if (Reticle && ReticleMaterial)
	{
		/* Reveal or hide the reticle and update its color. */
		Reticle->SetVisibility(bReveal, false);
		ReticleMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), bYellow ? ReticleColor_Hovered : ReticleColor_Selected);
	}
}

bool ABoardTile::IsAdjacentTo(bool bDiagonal, const ABoardTile* Other) const
{
	/* Store this tile and the given tile's coordinates in variables for readability. */
	const int ThisX = Coordinates.X, ThisY = Coordinates.Y;
	const int OtherX = Other->Coordinates.X, OtherY = Other->Coordinates.Y;

	/* If the tile is at a valid lateral location, return true. */
	if
	(
		/* 1 forward */
		(ThisY == OtherY + 1 && ThisX == OtherX) ||
		/* 1 backward */
		(ThisY == OtherY - 1 && ThisX == OtherX) ||
		/* 1 right */
		(ThisY == OtherY && ThisX == OtherX + 1) ||
		/* 1 left */
		(ThisY == OtherY && ThisX == OtherX - 1))
	{
		return true;
	}

	/* If we're also checking for diagonal adjacency and the tile is at a valid diagonal location, return true. */
	if (bDiagonal &&
	(
		/* 1 forward, 1 right */
		(ThisY == OtherY + 1 && ThisX == OtherX + 1) ||
		/* 1 forward, 1 left */
		(ThisY == OtherY + 1 && ThisX == OtherX - 1) ||
		/* 1 backward, 1 right */
		(ThisY == OtherY - 1 && ThisX == OtherX + 1) ||
		/* 1 backward, 1 left */
		(ThisY == OtherY - 1 && ThisX == OtherX - 1)))
	{
		return true;
	}

	/* If the given tile is not adjacent to this tile, return false. */
	return false;
}

void ABoardTile::SetOccupyingPiece_Implementation(AParentPiece* NewOccupyingPiece)
{
	/* Make sure that the new occupying piece is valid or a null pointer to reset it. */
	if (IsValid(NewOccupyingPiece) || NewOccupyingPiece == nullptr)
	{
		/* Change the occupying piece. Only the server can do this. */
		OccupyingPiece = NewOccupyingPiece;
	}
}
