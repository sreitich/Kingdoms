// Copyright Samuel Reitich 2022.

#pragma once

#include "Net/UnrealNetwork.h"
#include "UserDefinedData/Match_UserDefinedData.h"

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

	/* Plays a pop-up animation on each client, quickly scaling up the piece's size from 0.0 to 1.0, over a given duration. */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPiecePopUp(float Duration, bool bReverse);

		UFUNCTION(BlueprintImplementableEvent)
		void PlayPiecePopUp_BP(float Duration, bool bReverse);

	/* Rotates this piece to the rotation it was spawned with, facing away from its owner's starting position. */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ResetPieceRotation();

		/* Smoothly rotates the piece to the target rotation over a short time period. */
		UFUNCTION(BlueprintImplementableEvent)
		void InterpolatePieceRotation(FRotator CurrentRot, FRotator TargetRot);

	/* Flashes a given highlight onto the piece at a given strength for a given amount of time. */
	UFUNCTION(BlueprintCallable)
	void FlashHighlight(FLinearColor Color, float Brightness, float Duration);

		/* Fades to the target fresnel color and brightness at the given speed. Then waits for the duration and returns
		 * to the original color and brightness. */
		UFUNCTION(BlueprintImplementableEvent)
		void FlashHighlightTimeline(FLinearColor NewColor, float NewBrightness, FLinearColor OriginalColor, float
			OriginalBrightness, float Speed, float Duration);


/* Public functions overridden by each piece. */
public:

	/* Returns all tiles that this piece can move to (not accounting for other pieces or pathfinding). Overridden by each piece. */
	UFUNCTION(BlueprintPure, Category="Piece Range")
	virtual TArray<ABoardTile*> GetValidTiles();

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. Overridden by each piece. */
	UFUNCTION(BlueprintPure, Category="Piece Range")
	virtual bool TileIsInMoveRange(ABoardTile* Tile);


	/* Called when the player clicks the "Use Active" button to start selecting a target, or to immediately open a confirmation
	 * pop-up if the ability auto-targets. This is not overridden if the ability requires the player to manually select a target. */
	UFUNCTION(Category="ActiveAbility")
	virtual void OnActiveClicked();

	/* Returns all actors that this piece's active ability can target. If an active ability does not target anything,
	 * this returns the game state actor. Overridden by pieces with an active ability. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	virtual TArray<AActor*> GetValidActiveAbilityTargets();

	/* Called when the player selects a target to display and update a confirmation pop-up specific to that ability. */
	UFUNCTION(Category="ActiveAbility")
	virtual void StartActiveConfirmation(TArray<AActor*> Targets);
	
	/* Called when a piece uses an active ability, if it has one. Overridden by pieces with an active ability. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	virtual void OnActiveAbility(TArray<AActor*> Targets);


	/* Returns all actors that this piece's passive ability can target. Overridden by pieces with a passive ability. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	virtual TArray<AActor*> GetValidPassiveAbilityTargets();

	/* Called when a piece's passive ability is triggered, if it has one. Overridden by pieces with a passive ability. */
	UFUNCTION(Category="Passive Ability")
	virtual void OnPassiveAbility(TArray<AActor*> Targets);


/* Public accessors and modifiers. */
public:
	
	/* Getter for CurrentTile. */
	UFUNCTION(BlueprintPure, Category="Current Tile")
	FORCEINLINE ABoardTile* GetCurrentTile() const { return CurrentTile; }

	/* Server-only setter for CurrentTile. */
	UFUNCTION(BlueprintCallable, Category="Current Tile")
	bool SetCurrentTile(ABoardTile* NewTile);

	/* Getter for AttackInfo. */
	UFUNCTION(BlueprintPure, Category="Is Attacking?")
	FORCEINLINE FAttackInfo GetAttackInfo() const { return AttackInfo; }

	/* Server-only setter for AttackInfo. */
	UFUNCTION(BlueprintCallable, Category="Is Attacking?")
	void SetAttackInfo(FAttackInfo NewAttackInfo);


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


/* Public variables. */
public:

	/* This piece's unique identifier, set in-editor for each child piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Piece Info")
	FName PieceID = "0-00";

	/* Determines whether this piece's active ability animation is looped or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	bool bActiveAbilityLoops = false;


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

	/* Played when this piece takes damage without fighting back. This needs to have an "EndTakeDamage" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* TakingDamageAnimation;

	/* Played when this piece dies. This needs to have an "EndDeath" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* DeathAnimation;

	/* Played when this piece wins a fight and survives. This needs to have an "EndCelebration" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* CelebrationAnimation;

	/* Played when a fight ends in a deadlock. This needs to have an "EndDeadlockAnim" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* DeadlockAnimation;

	/* Played when this piece uses an active ability, if it has one. This needs to have an "EndActive" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* ActiveAbilityAnimation;

	/* Played when this piece uses a passive ability, if it has one. This needs to have an "EndPassive" animation notify. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* PassiveAbilityAnimation;

	/* The particle effect to spawn when this piece is damaged. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Effects")
	UParticleSystem* DamagedParticle;

	/* Which mesh component to play the damage particle effect on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Effects")
	USkeletalMeshComponent* DamagedParticleAttachment;

	/* The transform relative to the attached mesh that the damage particle effect should be played with. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Effects")
	FTransform DamagedParticleTransform;


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
	FLinearColor FriendlyFresnelColor = FColor(0.023529f, 0.423529f, 0.776471f, 1.0f);;

	/* The color that enemy pieces will be highlighted in. */
	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor EnemyFresnelColor = FColor(0.815686f, 0.015686f, 0.207843f, 1.0f);

	/* The widget that is created to confirm this piece's active ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Abilities")
	TSubclassOf<UUserWidget> ActiveAbilityConfirmationClass;


/* Protected runtime variables. These are variables that change during runtime, instead of being constants stored in the piece data table. */
protected:

	/* Pointer to the piece currently on this tile. */
	UPROPERTY(Replicated, EditInstanceOnly, Category="Piece Info")
	ABoardTile* CurrentTile;

	/* Tracks when this piece is participating in an attack as a defender. This is currently used for notifies to
	 * determine whether a piece is an attacker or defender without a copy of the attack information. */
	UPROPERTY(Replicated, EditInstanceOnly, Category="Attacking")
	bool bIsAttacking;

	/* Used to keep a copy of the attack information to continue passing down the chain of functions in the attack
	 * sequence when it gets interrupted by animation notifies. */
	UPROPERTY(Replicated, EditInstanceOnly, Category="Attacking")
	FAttackInfo AttackInfo;


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