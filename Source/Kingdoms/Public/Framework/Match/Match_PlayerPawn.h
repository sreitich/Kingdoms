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

	/* Calls the blueprint-implemented attack sequence with server authority. */
	UFUNCTION(Server, Reliable)
	void Server_Attack(const FAttackInfo InInfo);

	    /* Scripted attack sequence via blueprint. */
	    UFUNCTION(BlueprintImplementableEvent)
	    void Server_Attack_BP(const FAttackInfo InInfo);

			/* Calculates a target location and rotation and smoothly interpolates the player's camera to that location
			 * and rotation. */
			UFUNCTION(Client, Reliable, BlueprintCallable)
			void Client_MovePlayerCamera(const AParentPiece* Attacker, const AParentPiece* Defender, bool bReverse);

			/* Interpolates the camera between two locations. */
			UFUNCTION(BlueprintImplementableEvent)
			void InterpolatePlayerCamera(FVector TargetLocation, FRotator TargetRotation, float TargetArmLength, bool bReverse);

			/* Updates the given piece's ResetAfterMove variable on the server. This is in the player pawn because
			 * pieces don't have an owning client, so they can't have server methods. */
			UFUNCTION(Server, Reliable, BlueprintCallable, Category="Attacking")
			void Server_SetResetAfterMove(AParentPiece* PieceToUpdate, bool bNewReset);
	
	/* Calls active abilities with server authority. */
	UFUNCTION(Server, Reliable)
	void Server_UseActiveAbility(AParentPiece* AbilityUser, const TArray<AActor*>& Targets);


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

	/* The view transform to return to after an attack sequence ends. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attacking")
	FTransform OriginalViewTransform;

	/* The camera arm length to return to after an attack sequence ends. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attacking")
	float OriginalCameraArmLength;

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

	UPROPERTY()
	UUserWidget* ActiveAbilityConfirmationWidget;
	
};
