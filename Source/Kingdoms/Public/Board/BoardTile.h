// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardTile.generated.h"

/* Avoid circular dependency: a tile and its occupying piece need to have pointers to each other. */
class AParentPiece;
class URectLightComponent;

UCLASS()
class KINGDOMS_API ABoardTile : public AActor
{
	GENERATED_BODY()
	
/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	ABoardTile();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Replicates variables. */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Getter for OccupyingPiece. */
	UFUNCTION(BlueprintPure, Category="Occupying Piece")
	FORCEINLINE AParentPiece* GetOccupyingPiece() const { return OccupyingPiece; }

	/* Server-side setter for OccupyingPiece. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Occupying Piece")
	void SetOccupyingPiece(AParentPiece* NewOccupyingPiece);

	/* Called when the player starts hovering over this tile. */
	UFUNCTION(BlueprintCallable, Category="Board Tile")
	void OnBeginCursorOver(UPrimitiveComponent* Component);

	/* Called when the player stops hovering over this tile. */
	UFUNCTION(BlueprintCallable, Category="Board Tile")
	void OnEndCursorOver(UPrimitiveComponent* Component);
	
	/* Reveals or hides a yellow or green reticle, indicating that the player is hovering over this tile. */
	UFUNCTION(BlueprintCallable, Category="Board Tile")
	void UpdateReticle(bool bReveal, bool bYellow);

	UFUNCTION(BlueprintImplementableEvent, Category="Board Tile")
	void EnableGlow(bool bReverse);




	/* Sets the highlight depending on the occupying piece. */
	/* DEPRECATED: Tiles no longer highlight to signify occupation.  */
	UFUNCTION(BlueprintCallable, Category="Board Tile")
	void RefreshHighlight();	


/* Public variables. */
public:

	/* The tile's fixed coordinates. */
	UPROPERTY(EditInstanceOnly, Category="Board Tile")
	FTileCoordinates Coordinates;


/* Public components. */
public:

	/* Root of the actor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneRoot;

	/* The tile object itself. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Body;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Test;

	/* A monochromatic plane covering the tile to give the board a checkered appearance. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Checker;

	/* The highlight that indicates the occupied piece and valid tiles when moving or attacking. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Highlight;

	/* The reticle indicating if the player is hovering over this tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Reticle;

	/* Provides an emissive highlight onto this tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	URectLightComponent* EmissiveHighlight;

	/* An arrow that should point from player 1's spawn towards player 2's spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UArrowComponent* Arrow;


/* Public assets. */
public:

	/* Blank tile highlight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_Blank;

	/* The color of the reticle that appears when the player hovers over a tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	FLinearColor ReticleColor_Hovered;

	/* The color of the reticle that appears when the player selects a tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	FLinearColor ReticleColor_Selected;


	// Rework cutoff
	
	
	/* Tile occupied by friendly piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_Friendly;

	/* Tile occupied by enemy piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_Enemy;

	/* Tile valid for the currently selected piece to move to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_ValidMove;

	/* Tile is in range but not valid for the current action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_ValidRange;

	/* Tile occupied by a friendly piece that is a valid target of an ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_ValidFriendly;

	/* Tile is in range and occupied by a friendly piece but not valid for the current action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_ValidFriendlyRange;

	/* tile occupied by an enemy piece that is a valid target of an ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_ValidEnemy;

	/* Tile is in range and occupied by a enemy piece but not valid for the current action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_ValidEnemyRange;

	/* Tile that the mouse is currently hovering over. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_Target;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/* Refreshes this tile's highlight whenever its occupying piece changes. */
	UFUNCTION()
	void OnRep_OccupyingPiece();


/* Protected variables. */
protected:

	/* Pointer to the piece currently on this tile. */
	UPROPERTY(ReplicatedUsing=OnRep_OccupyingPiece, VisibleAnywhere, Category="Board Tile")
	AParentPiece* OccupyingPiece;

	/* This is the reticle mesh's material, used to change the material's parameters at runtime (usually the reticle color). */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Board Tile")
	UMaterialInstanceDynamic* ReticleMaterial;
	
	/* How bright the tile gets when highlighted. This doesn't affect tile highlight materials, just the light that
	 * emits when the tile is being highlighted as the source of a modifier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Board Tile")
	float TileLightBrightness = 3000.0f;

};
