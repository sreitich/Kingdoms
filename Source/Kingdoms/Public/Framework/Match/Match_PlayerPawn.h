// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Match_PlayerPawn.generated.h"

class UCameraComponent;
class UInputComponent;
class UMaterialInterface;
class UPieceNetworkingComponent;
class USceneComponent;
class USpringArmComponent;

class ABoardTile;
class AParentPiece;
class UPieceDragWidget;

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

		/* Clears the currently selected friendly piece, enemy piece, target piece, and tile, and removes all piece-information pop-ups if requested. */
		void ClearSelection(bool bPiece, bool bEnemyPiece, bool bTargetPiece, bool bTile, bool bHidePieceInfoWidgets);

		/* Deselects the given piece/tile if that piece/tile was selected for anything. */
		UFUNCTION(Client, Reliable)
		void Client_DeselectPieceOrTile(AParentPiece* PieceToDeselect, ABoardTile* TileToDeselect);

	/* Refreshes any piece info widget that is currently displaying this player's selected friendly or enemy piece. */
	UFUNCTION(Client, Reliable)
	void Client_RefreshPieceInfoWidgets() const;

	/* Hides any piece info widgets currently displaying the given piece. Used to hide piece info widgets displaying
	 * pieces that are dead. */
	UFUNCTION(Client, Reliable)
	void Client_HideWidgetDisplayingPiece(AParentPiece* PieceToHide) const;


/* Public piece networking functions. */
public:

	/* Flashes a given highlight onto a given piece on all clients */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_FlashHighlight(AParentPiece* PieceToHighlight, FLinearColor Color, float Brightness, float PlayRate, float Duration, bool bIndefiniteDuration);

	/* Calls the blueprint-implemented attack sequence with server authority. */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Attack(FAttackInfo InInfo, bool bMoveCamera);

	    /* Scripted attack sequence via blueprint. */
	    UFUNCTION(BlueprintImplementableEvent)
	    void BP_Attack(FAttackInfo InInfo, bool bMoveCamera);

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

			/* Destroys and cleans up the given piece. */
			UFUNCTION(Server, Reliable, BlueprintCallable)
			void KillPiece(AParentPiece* PieceToKill, AActor* Killer, bool bPieceReplaced);
	
	/* Calls active abilities with server authority. */
	UFUNCTION(Server, Reliable)
	void Server_UseActiveAbility(AParentPiece* AbilityUser, const TArray<AActor*>& Targets);


/* Public accessors. */
public:

	/* Returns this pawn's piece networking component, allowing other classes to call networked piece-related functions. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UPieceNetworkingComponent* GetPieceNetworkingComponent() const { return PieceNetworkingComponent; }


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

	/* Allows player pawn to manage networking for pieces, since pieces don't have owning connections. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UPieceNetworkingComponent* PieceNetworkingComponent;



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

	/* A pointer to the enemy piece that this player currently has selected. */
	UPROPERTY()
	AParentPiece* SelectedEnemyPiece = nullptr;

	/* A pointer to the piece that the player is currently targeting for an action. */
	UPROPERTY()
	AParentPiece* SelectedTargetPiece = nullptr;

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
