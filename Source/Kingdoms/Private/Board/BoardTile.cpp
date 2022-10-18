// Copyright Samuel Reitich 2022.


#include "Board/BoardTile.h"

#include "Components/RectLightComponent.h"
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
	Highlight->SetIsReplicated(true);

	Highlight->OnBeginCursorOver.AddDynamic(this, &ABoardTile::OnBeginCursorOver);


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

void ABoardTile::OnRep_OccupyingPiece()
{
	// /* Don't replicate tile highlights during piece setup. Otherwise, players will see where each other's pieces are. */
	// if (GetWorld()->GetGameState<AMatch_GameStateBase>()->GetCurrentMatchStatus() != E_SettingUpPieces ||
	// 	(IsValid(OccupyingPiece) && OccupyingPiece->GetInstigator()->IsLocallyControlled()) || !IsValid(OccupyingPiece)) 
	// {
	// 	/* Refresh this tile's highlight whenever its occupying piece changes. */
	// 	RefreshHighlight();
	// }
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

void ABoardTile::SetOccupyingPiece_Implementation(AParentPiece* NewOccupyingPiece)
{
	/* Make sure that the new occupying piece is valid or a null pointer to reset it. */
	if (IsValid(NewOccupyingPiece) || NewOccupyingPiece == nullptr)
	{
		/* Change the occupying piece. Only the server can do this. */
		OccupyingPiece = NewOccupyingPiece;

		/* The server won't call the OnRep function automatically. */
		// OnRep_OccupyingPiece();
	}
}

void ABoardTile::OnBeginCursorOver(UPrimitiveComponent* Component)
{
	UpdateReticle(true, true);
}

void ABoardTile::OnEndCursorOver(UPrimitiveComponent* Component)
{
	UpdateReticle(false, true);
}

void ABoardTile::UpdateReticle(bool bReveal, bool bYellow)
{
	/* Reveal or hide the reticle and update its color. */
	Reticle->SetVisibility(bReveal, false);
	ReticleMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), bYellow ? ReticleColor_Hovered : ReticleColor_Selected);
}

void ABoardTile::RefreshHighlight()
{
	// /* If this tile is occupied by a friendly piece... */
	// if (IsValid(OccupyingPiece) && OccupyingPiece->GetInstigator()->IsLocallyControlled())
	// {
	// 	Highlight->SetMaterial(0, Highlight_Friendly);
	// }
	// /* If this tile is occupied by an enemy piece... */
	// else if (IsValid(OccupyingPiece))
	// {
	// 	Highlight->SetMaterial(0, Highlight_Enemy);
	// }
	// /* If the tile is empty... */
	// else
	// {
	// 	Highlight->SetMaterial(0, Highlight_Empty);
	// }

	/* Make the tile blank. */
	Highlight->SetMaterial(0, Highlight_Blank);
}
