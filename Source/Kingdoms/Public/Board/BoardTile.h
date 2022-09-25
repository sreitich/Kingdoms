// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardTile.generated.h"

/* Avoid circular dependecy: a tile and its occupying piece need to have pointers to each other. */
class AParentPiece;

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

	/* Sets the highlight depending on the occupying piece. */
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
	class USceneComponent* SceneRoot;

	/* The tile object itself. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UStaticMeshComponent* Body;

	/* The highlight that indicates the occupied piece and valid tiles when moving or attacking. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UStaticMeshComponent* Highlight;

	/* An arrow that should point from player 1's spawn towards player 2's spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UArrowComponent* Arrow;


/* Public assets. */
public:

	/* Empty tile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Assets")
	UMaterialInstance* Highlight_Empty;

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

};
