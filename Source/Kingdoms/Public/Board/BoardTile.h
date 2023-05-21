// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/TargetInterface.h"
#include "BoardTile.generated.h"

class AMatch_PlayerState;
class AParentPiece;
class URectLightComponent;

UCLASS()
class KINGDOMS_API ABoardTile : public AActor, public ITargetInterface
{
	GENERATED_BODY()
	
/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	ABoardTile();

	/* Board tiles are compared to each other with their coordinates. Y-coordinates (rows) take priority. */
	bool operator<(const ABoardTile& Other) const;
	bool operator==(const ABoardTile& Other) const;
	
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

	/* Highlights this tile depending on its occupancy and alignment. */
	virtual void HighlightTarget(bool bTargetedByFriendly) override;

	/* Removes the highlight from this tile. */
	virtual void RemoveTargetHighlight() override;
	
	/* Reveals or hides a yellow or green reticle, indicating that the player is hovering over this tile. */
	UFUNCTION(BlueprintCallable, Category="Board Tile")
	void UpdateReticle(bool bReveal, bool bYellow);

	/* Enables/disables the emissive highlight, interpolating its brightness to glow the given color. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Board Tile")
	void UpdateHighlight(bool bReveal, float PlayRate, FLinearColor Color);

	/* Returns whether or not this tile is laterally (or diagonally) adjacent to the given tile. */
	UFUNCTION(BlueprintPure, Category="Board Tile")
	bool IsAdjacentTo(bool bDiagonal, const ABoardTile* Other) const;

	/* Returns true if the given tile is Forward units ahead of and Right units to the right of this tile, relative to
	 * this tile's occupying piece's owning player. */
	UFUNCTION(BlueprintPure, Category="Board Tile")
	bool CheckTilePosition(const ABoardTile* Other, int Forward, int Right) const;


/* Public variables. */
public:

	/* The tile's fixed coordinates. */
	UPROPERTY(EditInstanceOnly, Category="Board Tile")
	FTileCoordinates Coordinates;

	/* Whether or not this reticle is controlled by cursor hovering events. This determines if the reticle is updated
	 * when the tile is hovered over and if the reticle is removed when unhovered. */
	UPROPERTY(EditInstanceOnly, Category="Board Tile")
	bool bReticleControlledByCursor = true;


/* Public constants. */
public:

	/* Indicates valid tile targets for move destinations or abilities that target unoccupied tiles. */
	UPROPERTY(EditDefaultsOnly, Category="Colors")
	FLinearColor Highlight_ValidUnoccupiedTile;

	/* Indicates an enemy or enemy-occupied tile that is a valid target for a move (attack) or ability. */
	UPROPERTY(EditDefaultsOnly, Category="Colors")
	FLinearColor Highlight_Enemy;

	/* Indicates a friendly or friendly-occupied tile that is a valid target for an ability. */
	UPROPERTY(EditDefaultsOnly, Category="Colors")
	FLinearColor Highlight_Friendly;

	/* Indicates a tile that is within range of an action but is invalid for any reason. */
	UPROPERTY(EditDefaultsOnly, Category="Colors")
	FLinearColor Highlight_InvalidTile;

	/* The default play-rate with which to highlight tiles. */
	UPROPERTY(EditDefaultsOnly, Category="Highlights")
	float DefaultHighlightPlayRate = 4.0f;


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

	/* DEPRECATED: A monochromatic plane covering the tile to give the board a checkered appearance. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Checker;

	/* Crosshairs differentiating each tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* TileMarker;

	/* Emissive highlight indicating the state of the tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Highlight;

	/* The reticle indicating if the player is hovering over this tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Reticle;

	/* DEPRECATED: The emissive highlight that indicates the occupied piece and valid tiles when moving or attacking. */
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


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


/* Protected variables. */
protected:

	/* Pointer to the piece currently on this tile. */
	UPROPERTY(Replicated, VisibleAnywhere, Category="Board Tile")
	AParentPiece* OccupyingPiece;

	/* This is the highlight mesh's material, used to change the material's parameters at runtime (i.e. the color and brightness). */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Board Tile")
	UMaterialInstanceDynamic* HighlightMaterial;

	/* This is the reticle mesh's material, used to change the material's parameters at runtime (usually the reticle color). */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Board Tile")
	UMaterialInstanceDynamic* ReticleMaterial;
	
	/* How bright the tile gets when highlighted. This doesn't affect tile highlight materials, just the light that
	 * emits when the tile is being highlighted as the source of a modifier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Board Tile")
	float TileLightBrightness = 3000.0f;

};
