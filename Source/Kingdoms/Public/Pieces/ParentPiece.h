// Copyright Samuel Reitich 2022.

#pragma once

#include "Net/UnrealNetwork.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Components/TimelineComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParentPiece.generated.h"

class ABoardTile;
class UCurveFloat;
class UPopUpLocationComponent;

class UMatch_AbilityInfoPopup;

UCLASS()
class KINGDOMS_API AParentPiece : public ACharacter
{

/* This piece's controller needs to access its members. */
friend class APieceAIController;

	GENERATED_BODY()
	
/* Public functions. */
public:

	/* Sets default values for this character's properties. */
	AParentPiece();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Replicates variables. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Called when the player starts hovering over this piece. */
	UFUNCTION()
	void OnBeginCursorOver(UPrimitiveComponent* Component);

	/* Called when the player stops hovering over this piece. */
	UFUNCTION()
	void OnEndCursorOver(UPrimitiveComponent* Component);

	/* Returns this piece's alignment relative to the local player. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAlignment GetLocalAlignment() const { return GetInstigator()->IsLocallyControlled() ? E_Friendly : E_Hostile; }

	/* Plays a piece pop-up animation, scaling up the piece's size from 0.0 to 1.0 over the given duration. */
	UFUNCTION(BlueprintCallable)
	void PlayPiecePopUp(float Duration, bool bReverse);

	/* Smoothly rotates the piece to the target rotation over a short time period. If bMoveWhenFinished is true, the
	 * piece is moved to the target tile when it finishes rotating. If bResetStateWhenFinished is true, the player's
	 * state is reset after the piece finishes rotating. */
	UFUNCTION(BlueprintCallable)
	void InterpolatePieceRotation(ABoardTile* NewTile, FRotator OriginalRot, FRotator TargetRot, bool bMoveWhenFinished, bool bResetStateWhenFinished);

		/* Rotates this piece to the rotation it was spawned with, facing away from its owner's starting position. */
		UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_ResetPieceRotation();

	/* Flashes a given highlight onto the piece at a given strength for a given amount of time. Normal brightness is 4.0.
	 * Highlighted brightness is 20.0 for action/ability targets, 10.0 for effects/modifiers. Standard play-rate is
	 * 0.25. Standard duration is 0.5. */
	UFUNCTION(BlueprintCallable)
	void FlashHighlight(FLinearColor Color, float Brightness, float PlayRate, float Duration, bool bIndefiniteDuration);

	/* Spawns and animates a modifier pop-up with the appropriate information at this piece's pop-up location. */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_CreateModifierPopUp(int ValueChange, bool bStrength, bool bFlashHighlight);

	/* Returns true if there are no occupied tiles between this piece's current tile and a given tile. */
	UFUNCTION()
	bool PathToTileIsClear(ABoardTile* TargetTile);


/* Public functions overridden by each piece. */
public:

	/* Called when the game starts, ensuring that all pieces are placed onto their starting tiles. Overridden by pieces
	 * that require code to be executed when the game starts. */
	UFUNCTION(Category="Piece Initialization")
	virtual void OnGameStart();


	/* Returns all valid tiles that this piece can currently move to. Checks for validity. Overridden by each piece. */
	UFUNCTION(BlueprintPure, Category="Movement")
	virtual TArray<ABoardTile*> GetValidMoveTiles();

	/* Tests if the given tile's coordinates match any of this piece's movement patterns. Overridden by each piece. */
	UFUNCTION(BlueprintPure, Category="Movement")
	virtual bool TileIsInMoveRange(ABoardTile* Tile);


	/* Called when the player clicks the "Use Active" button to create an ability confirmation widget.
	 * Default implementation highlights all valid targets. Default implementation cannot create the ability
	 * confirmation widget because the parent class of the widget changes per piece and thus cannot be updated. */
	UFUNCTION(Category="Active Ability")
	virtual void OnActiveClicked();

	/* Called when the player clicks an actor while in the SelectingTarget_ActiveAbility state. Default implementation
	 * calls Piece_UpdateActiveConfirmation to update the active ability confirmation widget. Overridden by each piece. */
	UFUNCTION(Category="Active Ability")
	virtual void OnTargetSelected(TArray<AActor*> Targets);

		/* Updates this piece's active ability confirmation widget. Called by OnTargetSelected to update the piece-specific
		 * active ability confirmation widget when the player selects a target for the ability. */
		UFUNCTION(Category="Active Ability")
		virtual void Piece_UpdateActiveConfirmation(TArray<AActor*> Targets);

	/* Returns all actors within the active ability range that this piece's active ability can currently target. Checks
	 * for validity. Overridden by pieces with an active ability. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	virtual TArray<AActor*> GetValidActiveAbilityTargets();

		/* Returns the tiles within range of this piece's active ability, without checking for validity. Overridden by
		 * pieces with an active ability.*/
		UFUNCTION(BlueprintPure, Category="Active Ability")
		virtual TArray<ABoardTile*> GetActiveAbilityRange();

	/* Called when a piece uses an active ability, if it has one. Called on server by default. Default implementation
	 * triggers cool-downs and decrements uses as needed. Overridden by pieces with an active ability. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	virtual void OnActiveAbility(TArray<AActor*> Targets);

	/* If one of this piece's ability has a lasting effect (e.g. a modifier), this is called when that effect ends.
	 * Overridden by pieces with that need to perform code when an effect ends. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	virtual void OnAbilityEffectEnded(TArray<AActor*> Targets);


	/* Called when a piece's passive ability is triggered, if it has one. Overridden by pieces with a passive ability. */
	UFUNCTION(Category="Passive Ability")
	virtual void OnPassiveAbility(TArray<AActor*> Targets);


	/* Called when this piece is killed by an attack or ability. */
	UFUNCTION(Category="Death")
	virtual void OnDeath(AActor* Killer);


/* Public accessors and mutators. */
public:
	
	/* Getter for CurrentTile. */
	UFUNCTION(BlueprintPure, Category="Current Tile")
	FORCEINLINE ABoardTile* GetCurrentTile() const { return CurrentTile; }

	/* Sets this piece's current tile locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Current Tile")
	void SetCurrentTile(ABoardTile* NewTile);

	/* Getter for DynamicMaterial. */
	UFUNCTION(BlueprintPure, Category="Materials")
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterial() const { return DynamicMaterial; }

	/* Getter for AttackInfo. */
	UFUNCTION(BlueprintPure, Category="Is Attacking?")
	FORCEINLINE FAttackInfo GetAttackInfo() const { return AttackInfo; }

	/* Sets the information for the current attack locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Is Attacking?")
	void SetAttackInfo(FAttackInfo NewAttackInfo);


	/* Getter for TemporaryModifiers. Returns a reference so that this array can be mutated. */
	UFUNCTION(BlueprintCallable, Category="Modifiers")
	FORCEINLINE TArray<FModifier>& GetTemporaryModifiers() { return TemporaryModifiers; }

	/* Updates this piece's strength and armor. This lets the strength and armor variables stay protected, so they can
	 * only be updated by applying modifiers. This is public to be used by the piece networking component. */
	UFUNCTION()
	void OnRep_TemporaryModifiers(TArray<FModifier> OldTemporaryModifiers);


	/* Getter for this piece's current strength stat. */
	UFUNCTION(BlueprintPure, Category="Piece Stats")
	FORCEINLINE int GetCurrentStrength() const { return CurrentStrength; }

	/* Getter for this piece's current armor stat. */
	UFUNCTION(BlueprintPure, Category="Piece Stats")
	FORCEINLINE int GetCurrentArmor() const { return CurrentArmor; }


	/* Getter for this piece's current passive cooldown. */
	UFUNCTION(BlueprintPure, Category="Passive Ability")
	FORCEINLINE int GetPassiveCD() const { return PassiveCD; }

	/* Sets this piece's current passive ability cooldown locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	void SetPassiveCD(int NewPassiveCD);

	/* Getter for this piece's remaining passive ability uses. */
	UFUNCTION(BlueprintPure, Category="Passive Ability")
	FORCEINLINE int GetPassiveUses() const { return PassiveUses; }

	/* Sets this piece's remaining passive ability uses locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	void SetPassiveUses(int NewPassiveUses);

	/* Getter for this piece's current active ability cooldown. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	FORCEINLINE int GetActiveCD() const { return ActiveCD; }

	/* Sets this piece's current active ability cooldown locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	void SetActiveCD(int NewActiveCD);

	/* Getter for this piece's remaining active ability uses. */
	UFUNCTION(BlueprintPure, Category="Active Ability")
	FORCEINLINE int GetActiveUses() const { return ActiveUses; }

	/* Sets this piece's remaining active ability uses locally and on the server. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	void SetActiveUses(int NewActiveUses);


/* Public variables. */
public:

	/* This piece's unique identifier, set in-editor for each child piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Piece Info")
	FName PieceID = "0-00";
	
	/* The default brightness of fresnel emissives. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Piece Info")
	float DefaultFresnelStrength = 2.0f;

	/* The brightness of fresnels when strengthened to indicate that this piece is selected. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Piece Info")
	float SelectedFresnelStrength = 6.0f;

	/* The brightness of fresnels when strengthened to indicate a buff or debuff. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Piece Info")
	float FlashFresnelStrength = 10.0f;
	
	/* The widget to display when hovering over this piece's passive ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widgets")
	TSubclassOf<UUserWidget> PassiveAbilityInfoWidget;

	/* The widget to display when hovering over this piece's active ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widgets")
	TSubclassOf<UUserWidget> ActiveAbilityInfoWidget;

	/* Determines whether this piece's active ability animation is looped or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	bool bActiveAbilityLoops = false;

	/* Tracks whether a modifier's pop-up animation is currently playing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	bool bIsModifierPopUpPlaying = false;

	/* How long modifier pop-ups last for. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Modifiers")
	float ModifierPopUpDuration = 1.0f;

	/* The play-rate of the highlight timeline when indicating modifiers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Modifiers")
	float ModifierFlashPlayRate = 0.5f;

	/* How long modifier flashes last for. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Modifiers")
	float ModifierFlashDuration = 0.25f;

	/* A reference to this piece's active ability confirmation widget, if it has one. Prevents the widget from being
	 * created multiple times. */
	UPROPERTY()
	UUserWidget* ActiveAbilityConfirmationWidget;


/* Public constants and asset references. */
public:

	/* Pointer to the piece data table, used to retrieve this piece's statistics. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	UDataTable* PieceDataTable;

	/* The curve used for pieces' pop-up animations. */
	UPROPERTY(EditAnywhere, Category="Curves")
	UCurveFloat* PiecePopUpCurve;

	/* The curve used to smoothly rotate pieces. */
	UPROPERTY(EditAnywhere, Category="Curves")
	UCurveFloat* PieceRotationCurve;
	
	/* The curve used for highlighting pieces. */
	UPROPERTY(EditAnywhere, Category="Curves")
	UCurveFloat* PieceHighlightCurve;

	/* The color that friendly pieces will be highlighted in. */
	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor FriendlyFresnelColor = FLinearColor(0.023529f, 0.423529f, 0.776471f, 1.0f);

	/* The color that enemy pieces will be highlighted in. */
	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor EnemyFresnelColor = FLinearColor(0.815686f, 0.015686f, 0.207843f, 1.0f);

	/* The color that this piece flashes when gaining a buff. */
	UPROPERTY(EditDefaultsOnly, Category="Highlights")
	FLinearColor BuffColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);

	/* The color that this piece flashes when gaining a debuff or losing a buff. */
	UPROPERTY(EditDefaultsOnly, Category="Highlights")
	FLinearColor DebuffColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

	/* This piece's idle/walk blend space. Used to interpolate and play this piece's unique idle and walk animations. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UBlendSpaceBase* IdleWalkBS;

	/* Played when this piece attacks. This needs to have an "EndAttack" animation notify. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations")
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

	/* The particle effect to spawn on opponent pieces when this piece attacks them. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Effects")
	UParticleSystem* DamagingParticle;

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


	/* Called every tick of the pop-up timeline to update the target piece's transform. */
	UFUNCTION()
	void PopUpTimelineTick(float Value);


	/* Called every tick of the piece rotation timeline to update the piece's rotation. */
	UFUNCTION()
	void RotationTimelineTick(float Value);

	/* Called at the end of the piece rotation timeline to reset the variables used throughout the timeline. */
	UFUNCTION()
	void RotationTimelineEnd();


	/* Called every tick of the piece highlight timeline to update the piece's highlight. */
	UFUNCTION()
	void HighlightTimelineTick(float Value);

	/* If the current piece highlight is NOT indefinite, waits for the given duration before calling HighlightDurationEnd. */
	UFUNCTION()
	void HighlightTimelineEnd();

	/* Reverses the current piece highlight. */
	UFUNCTION()
	void HighlightDurationEnd();

	
	/* Refreshes any piece info widget displaying this piece. */
	UFUNCTION()
	void OnRep_CurrentStrength();

	/* Refreshes any piece info widget displaying this piece. */
	UFUNCTION()
	void OnRep_CurrentArmor();

	/* Refreshes any piece info widget displaying this piece. */
	UFUNCTION()
	void OnRep_PassiveCooldown();

	/* Refreshes any piece info widget displaying this piece. */
	UFUNCTION()
	void OnRep_PassiveUses();

	/* Refreshes any piece info widget displaying this piece. */
	UFUNCTION()
	void OnRep_ActiveCooldown();

	/* Refreshes any piece info widget displaying this piece. */
	UFUNCTION()
	void OnRep_ActiveUses();

	/* Called when this piece finishes moving to a new tile. Overridden by pieces that require code to be executed upon
	 * moving to a new tile. */
	UFUNCTION(BlueprintCallable)
	virtual void OnMoveToTileCompleted();


/* Protected accessors and modifiers. */
protected:

	/* Sets this piece's current tile on the server. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Current Tile")
	void Server_SetCurrentTile(ABoardTile* NewTile);

	/* Sets the information for the current attack on the server. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Is Attacking?")
	void Server_SetAttackInfo(FAttackInfo NewAttackInfo);


	/* Sets this piece's passive ability cooldown on the server. */
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	void Server_SetPassiveCD(int NewPassiveCD);

	/* Sets this piece's remaining passive ability uses on the server. */
	UFUNCTION(BlueprintCallable, Category="Passive Ability")
	void Server_SetPassiveUses(int NewPassiveUses);

	/* Sets this piece's active ability cooldown on the server. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	void Server_SetActiveCD(int NewActiveCD);

	/* Sets this piece's remaining active ability uses on the server. */
	UFUNCTION(BlueprintCallable, Category="Active Ability")
	void Server_SetActiveUses(int NewActiveUses);


/* Protected variables. */
protected:

	/* Piece pop-up animation timeline variables. */
	FTimeline PopUpTimeline;

	/* Piece rotation timeline variables. */
	FTimeline RotationTimeline;
	bool bRotationStartedMoving;
	/* Rotation timeline parameters. */
	UPROPERTY()
	ABoardTile* RotationNewTile;
	FRotator OriginalRotationRot;
	FRotator TargetRotationRot;
	bool bRotationMoveWhenFinished;
	bool bRotationResetStateWhenFinished;

	/* Highlight timeline variables. */
	FTimeline HighlightTimeline;
	TEnumAsByte<ETimelineDirection::Type> HighlightTimelineDirection;
	/* Highlight timeline parameters. */
	FLinearColor OriginalHighlightColor;
	FLinearColor TargetHighlightColor;
	float OriginalHighlightBrightness;
	float TargetHighlightBrightness;
	float HighlightPlayRate;
	float HighlightDuration;
	bool bIndefiniteHighlightDuration;


/* Protected constants and asset references. */
protected:

	/* The material used for this piece's skeletal meshes, including the parent mesh. Used to edit the fresnel during runtime. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Materials")
	UMaterialInstanceDynamic* DynamicMaterial;

	/* The widget that is created to confirm this piece's active ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Abilities")
	TSubclassOf<UUserWidget> ActiveAbilityConfirmationClass;

	/* The class to be spawned when creating a modifier pop-up actor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Pop-Ups")
	TSubclassOf<class AModifierBoardPopup> ModifierBoardPopupClass;


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

	/* Every modifier currently applied to this piece's statistics. */
	UPROPERTY(ReplicatedUsing=OnRep_TemporaryModifiers, EditInstanceOnly, BlueprintReadOnly, Category="Piece Info")
	TArray<FModifier> TemporaryModifiers;


	/* This piece's current strength with modifiers. */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentStrength, EditInstanceOnly, BlueprintReadOnly, Category="Piece Stats")
	int CurrentStrength;

	/* This piece's current armor with modifiers. */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentArmor, EditInstanceOnly, BlueprintReadOnly, Category="Piece Stats")
	int CurrentArmor;

	/* How long before this piece's passive ability can be used again. */
	UPROPERTY(ReplicatedUsing=OnRep_PassiveCooldown, EditInstanceOnly, BlueprintReadOnly, Category="Piece Stats")
	int PassiveCD;

	/* How many uses this piece's passive ability has left. */
	UPROPERTY(ReplicatedUsing=OnRep_PassiveUses, EditInstanceOnly, BlueprintReadOnly, Category="Piece Stats")
	int PassiveUses;

	/* How long before this piece's passive ability can be used again. */
	UPROPERTY(ReplicatedUsing=OnRep_ActiveCooldown, EditInstanceOnly, BlueprintReadOnly, Category="Piece Stats")
	int ActiveCD;

	/* How many uses this piece's active ability has left. */
	UPROPERTY(ReplicatedUsing=OnRep_ActiveUses, EditInstanceOnly, BlueprintReadOnly, Category="Piece Stats")
	int ActiveUses;

};