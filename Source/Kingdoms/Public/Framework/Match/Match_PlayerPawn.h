// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Match_PlayerPawn.generated.h"

class UInputComponent;
class UMaterialInterface;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;

class UPieceDragWidget;
class AParentPiece;
class ABoardTile;

UCLASS()
class KINGDOMS_API AMatch_PlayerPawn : public APawn
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	AMatch_PlayerPawn();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;
	
	/* Called to bind functionality to input. */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/* Called whenever the player clicks. */
	UFUNCTION()
	void Interact();

		/* Clears the currently selected piece and removes all piece-information pop-ups. */
		void ClearSelection(bool bDeselect);

	/* Initiates an attack with given parameters. Moves attacker to the defender if they're too far away. Calls 
	 * WaitForPieceProximity. Calls Client_SetUpAttack on each client. */
	UFUNCTION(Server, Reliable)
	void Server_Attack(const FAttackInfo InInfo);

		/* Attack Phase 1. Waits for when the attacker and defender are close enough before calling
		 * Client_InitiateAttack on each client with server authority. */
		UFUNCTION(BlueprintImplementableEvent)
		void WaitForPieceProximity(FAttackInfo InInfo);

		/* Attacking Phase 1. Disables input on the client. */
		UFUNCTION(Client, Reliable)
		void Client_SetUpAttack();

		/* Attacking Phase 2. Initializes the client's visual aspects of an attack: changes camera angle, highlights
		 * pieces, creates an attack graphic, creates billboards, and calls Server_AnimateAttack and
		 * Client_AnimateAttack when the camera finishes moving. */
		UFUNCTION(Client, Reliable, BlueprintCallable)
		void Client_InitiateAttack(FAttackInfo InInfo);
	
			/* Calculates a target location and rotation and smoothly interpolates the player's camera to that location
			 * and rotation. Calls Server_AnimateAttack after a brief duration when the camera finishes moving. */
			UFUNCTION()
			void MovePlayerCamera(FAttackInfo InInfo);

				/* Interpolates the camera between two locations. */
				UFUNCTION(BlueprintImplementableEvent)
				void InterpolateCamera(FVector StartingLocation, FVector EndingLocation, FRotator StartingRotation, FRotator EndingRotation, float StartingArmLength, float EndingArmLength, bool bReverse, FAttackInfo InInfo);

			/* Spawns a billboard popup at the given location. */
			UFUNCTION()
			void SpawnTempBillboardPopup(EAttackBillboardPopUpTexture DisplayedTexture, FVector Location, float Duration) const;

		/* Attacking Phase 3. Animates both pieces' attacks for both players. Animations need to be replicated because
		 * server-side events are triggered via animation notifies. */
		UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_AnimateAttack(FAttackInfo InInfo);

			/* Triggers the attacking animation of the given pieces on each client. */
			UFUNCTION(NetMulticast, Reliable)
			void Multicast_AnimateAttack(FAttackInfo InInfo);

					/* Animates a "damage-taken" particle effect and a "damage-taken" animation when necessary. */
					UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
					void Multicast_AnimateDamage(FAttackInfo InInfo, bool bAttackerDamaged);


/* Public variables. */
public:

	/* Used to give a reference to a spawned piece to a drag widget in order to drag the piece around. */
	UPROPERTY()
	AActor* DraggedPiece;


/* Public components. */
public:

	/* Root of the actor. */
	UPROPERTY(EditAnywhere, Category="Components")
	USceneComponent* SceneRoot;

	/* Manages camera angle and position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USpringArmComponent* SpringArm;

	/* Player's in-game viewport. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UCameraComponent* Camera;


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/* Called when the player interacts while placing their pieces. */
	void Interact_PlacingPieces(FHitResult InteractionHit);
	
	/* Called whenever the player interacts while waiting for their turn. */
	void Interact_WaitingForTurn(FHitResult InteractionHit);

	/* Called whenever the player interacts while it's their turn. */
	void Interact_SelectingPiece(FHitResult InteractionHit);

	/* Called whenever the player interacts while choosing a destination for a piece to move to. */
	void Interact_SelectingTargetMove(FHitResult InteractionHit);

	/* Called whenever the player interacts while choosing a target for the currently selected piece's active ability. */
	void Interact_SelectingTargetActiveAbility(FHitResult InteractionHit);

	/* Called whenever the player interacts while choosing a target for the currently selected piece's passive ability. */
	void Interact_SelectingTargetPassiveAbility(FHitResult InteractionHit);

	/* Called when the player releases the "interact" button. Used to release pieces being dragged. */
	UFUNCTION()
	void Interact_Released();


/* Protected variables. */
protected:

	/* Determines whether the player is currently dragging a piece in the setup phase. */
	UPROPERTY()
	bool bIsDraggingPiece;
	
	/* If the player is currently looking from a top-down perspective. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool bIsTopDown;

	/* A pointer to the piece that this player currently has selected. This is also used by the piece drag widget to get the spawned piece. */
	UPROPERTY()
	AParentPiece* SelectedPiece = nullptr;

	/* A pointer to the tile that this player currently has selected. */
	UPROPERTY()
	ABoardTile* SelectedTile = nullptr;


/* Protected class types to access blueprint classes. */
protected:

	UPROPERTY(EditDefaultsOnly, Category="Class Types")
	TSubclassOf<UUserWidget> PieceDragWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Class Types")
	TSubclassOf<class ATextureBoardPopup> TextureBoardPopupClass;


/* Protected widget class references. */
protected:

	UPROPERTY()
	UPieceDragWidget* PieceDragWidget;
	
};
