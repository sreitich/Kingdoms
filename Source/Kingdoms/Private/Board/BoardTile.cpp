// Copyright Samuel Reitich 2022.


#include "Board/BoardTile.h"

#include "Components/RectLightComponent.h"
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
	if (bReticleControlledByCursor && UGameplayStatics::GetPlayerPawn(this, 0)->InputEnabled())
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

			/* When a player is selecting a target for a passive ability, display a green reticle when hovering over a
			 * valid target tile or piece and a yellow reticle over all other tiles. */
			case E_SelectingTarget_PassiveAbility:
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

void ABoardTile::SetOccupyingPiece_Implementation(AParentPiece* NewOccupyingPiece)
{
	/* Make sure that the new occupying piece is valid or a null pointer to reset it. */
	if (IsValid(NewOccupyingPiece) || NewOccupyingPiece == nullptr)
	{
		/* Change the occupying piece. Only the server can do this. */
		OccupyingPiece = NewOccupyingPiece;
	}
}
