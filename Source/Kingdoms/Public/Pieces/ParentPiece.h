// Copyright Samuel Reitich 2022.

#pragma once

#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParentPiece.generated.h"

class ABoardTile;
class UCurveFloat;
class UPopUpLocationComponent;

UCLASS()
class KINGDOMS_API AParentPiece : public ACharacter
{
	GENERATED_BODY()
	
/* Public functions. */
public:

	/* Sets default values for this character's properties. */
	AParentPiece();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Replicates variables. */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Flashes a given highlight onto the piece at a given strength for a given amount of time. */
	UFUNCTION()
	void FlashHighlight(FLinearColor Color, float Brightness, float Duration);

		/* Fades to the target fresnel color and brightness at the given speed. Then waits for the duration and returns
		 * to the original color and brightness. */
		UFUNCTION(BlueprintImplementableEvent)
		void FlashHighlightTimeline(FLinearColor NewColor, float NewBrightness, FLinearColor OriginalColor, float
			OriginalBrightness, float Speed, float Duration);


/* Public functions: accessors and modifiers. */
public:

	/* Retrieves this piece's piece ID. This function is overridden by each piece with an ID that is unique to that
	 * piece. This is used in place of a member variable and has complete control over this piece's ID. */
	// UFUNCTION(BlueprintPure, Category="Data")
	virtual FName GetPieceID();

	/* Getter for CurrentTile. */
	UFUNCTION(BlueprintPure, Category="Current Tile")
	FORCEINLINE ABoardTile* GetCurrentTile() const { return CurrentTile; }

	/* Server-only setter for CurrentTile. */
	UFUNCTION(BlueprintCallable, Category="Current Tile")
	bool SetCurrentTile(ABoardTile* NewTile);

	/* Getter for CurrentStrength. */
	UFUNCTION(BlueprintPure, Category="Piece Stats")
	FORCEINLINE int GetCurrentStrength() const { return CurrentStrength; }

	/* Server-only setter for CurrentStrength. */
	UFUNCTION(BlueprintCallable, Category="Piece Stats")
	bool SetCurrentStrength(int NewStrength);

	/* Getter for CurrentArmor. */
	UFUNCTION(BlueprintPure, Category="Piece Stats")
	FORCEINLINE int GetCurrentArmor() const { return CurrentArmor; }

	/* Server-only setter for CurrentArmor. */
	UFUNCTION(BlueprintCallable, Category="Piece Stats")
	bool SetCurrentArmor(int NewArmor);

	/* Getter for PassiveCD. */
	UFUNCTION(BlueprintPure, Category="Passive Ability")
	FORCEINLINE int GetPassiveCD() const { return PassiveCD; }

	/* Server-only setter for PassiveCD. */
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	bool SetPassiveCD(int NewPassiveCD);

	/* Getter for PassiveUses. */
	UFUNCTION(BlueprintPure, Category="Passive Ability")
	FORCEINLINE int GetPassiveUses() const { return PassiveUses; }

	/* Server-only setter for PassiveUses. */
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	bool SetPassiveUses(int NewPassiveUses);

	/* Getter for ActiveCD. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	FORCEINLINE int GetActiveCD() const { return ActiveCD; }

	/* Server-only setter for ActiveCD. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	bool SetActiveCD(int NewActiveCD);

	/* Getter for ActiveUses. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	FORCEINLINE int GetActiveUses() const { return ActiveUses; }

	/* Server-only setter for ActiveUses. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	bool SetActiveUses(int NewActiveUses);

	/* Finds all tiles that this piece can move to (not accounting for other pieces or pathfinding). */
	virtual TArray<ABoardTile*> GetValidTiles() final;

	/* Pure function that tests if the given tile's coordinates match any of this piece's movement patterns. Overridden by each piece. */
	virtual bool TileIsInMoveRange(ABoardTile* Tile);


/* Public assets. */
public:

	/* Pointer to the piece data table, used to retrieve this piece's statistics. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	class UDataTable* PieceDataTable;

	/* This piece's idle/walk blend space. Used to interpolate and play this piece's unique idle and walk animations. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UBlendSpaceBase* IdleWalkBS;

	/* Played when this piece attacks. This needs to have an "EndAttack" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* AttackAnimation;


/* Public components. */
public:

	/* Root of the actor. Character already comes with capsule and skeletal mesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneRoot;

	/* Root of the actor. Character already comes with capsule and skeletal mesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UPopUpLocationComponent* PopUpLocationComponent;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


/* Protected constant variables. */
protected:

	/* The material used for this piece's skeletal meshes, including the parent mesh. Used to edit the fresnel during runtime. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Materials")
	UMaterialInstanceDynamic* DynamicMaterial;

	/* The color that friendly pieces will be highlighted in. */
	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor FriendlyFresnelColor;

	/* The color that enemy pieces will be highlighted in. */
	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor EnemyFresnelColor;


/* Protected runtime variables. These are variables that change during runtime, instead of being constants stored in the piece data table. */
protected:

	/* Pointer to the piece currently on this tile. */
	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadOnly, Category="Piece Info")
	ABoardTile* CurrentTile;


	/* EditAnywhere enabled for playtesting. These will be changed to Replicated, EditInstanceOnly, BlueprintReadOnly
	 * later. */

	/* This piece's current strength with modifiers. */
	UPROPERTY(Replicated, EditAnywhere, Category="Piece Stats")
	int CurrentStrength;

	/* This piece's current armor with modifiers. */
	UPROPERTY(Replicated, EditAnywhere, Category="Piece Stats")
	int CurrentArmor;

	/* How long before this piece's passive ability can be used again. */
	UPROPERTY(Replicated, EditAnywhere, Category="Piece Stats")
	int PassiveCD;

	/* How many uses this piece's passive ability has left. */
	UPROPERTY(Replicated, EditAnywhere, Category="Piece Stats")
	int PassiveUses;

	/* How long before this piece's passive ability can be used again. */
	UPROPERTY(Replicated, EditAnywhere, Category="Piece Stats")
	int ActiveCD;

	/* How many uses this piece's active ability has left. */
	UPROPERTY(Replicated, EditAnywhere, Category="Piece Stats")
	int ActiveUses;

};