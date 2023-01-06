// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/PieceData_UserDefinedData.h"
#include "UserDefinedData/SaveGameData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Match_PlayerController.generated.h"

class ABoardTile;
class AParentPiece;
class UMatch_AttackConfirmation;
class UMatch_AttackGraphic;
class UMatch_BaseWidget;
class UMatch_MoveConfirmation;
class UMatch_PieceInfoWidget;
class UMatchSetup_PlacePieces;
class UMatchSetup_SelectArmy;
class UServerCommunicationComponent;

enum EAttackGraphicAnimation;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMatch_PlayerController : public APlayerController
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	AMatch_PlayerController();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Getter for the server communication component. */
	UFUNCTION(BlueprintPure, Category="Networking")
	FORCEINLINE UServerCommunicationComponent* GetServerCommunicationComponent() const { return ServerCommunicationComponent; }


/* Public UI-managing functions. */
public:

	/* Creates or destroys the army selection widget. */
	UFUNCTION(Client, Reliable, Category="User Inteface")
	void CreateSelectArmyWidget(bool bDestroy);

	/* Creates or destroys the place pieces widget. */
	UFUNCTION(Client, Reliable, Category="User Inteface")
	void CreatePlacePiecesWidget(FArmyPresetStruct SelectedArmy, bool bDestroy);

	/* Creates, destroys, hides, or reveals the base widget. */
	UFUNCTION(Client, Reliable, Category="User Inteface")
	void CreateBaseWidget(bool bDestroy, bool bHide);

	/* Updates the displayed time every second by getting the time from the game state. */
	UFUNCTION(Category="User Interface")
	void UpdateTimer() const;

	/* Updates the currently displayed map name. Gets the name from the game state, or can be passed a map name with NewName. */
	UFUNCTION(Category="User Interface")
	void UpdateMapName(FName NewName) const;

	/* Updates the requested action indicator to indicate whether or not an action has been used. */
	UFUNCTION(Category="User Interface")
	void UpdateActionIndicator(bool bUsed, bool bMoveAction) const;

	/* Enables or disables the "end turn" button. */
	UFUNCTION(BlueprintCallable, Category="User Interface")
	void UpdateEndTurnButton(bool bEnable) const;

	/* Reveals and updates or hides one of the piece info widgets. */
	UFUNCTION(Category="User Interface")
	void UpdatePieceInfoWidget(AParentPiece* NewPiece, EAlignment Alignment, bool bHide) const;
	
		/* Refreshes both piece info widgets, if they have been created. */
		UFUNCTION(Category="User Interface")
		void RefreshPieceInfoWidgets(AParentPiece* PieceToRefresh) const;

		/* Hides any piece info widgets currently displaying the given piece. Used to hide piece info widgets displaying
		 * pieces that are dead. */
		UFUNCTION(Category="User Interface")
		void HideWidgetDisplayingPiece(AParentPiece* PieceToHide) const;

	/* Creates or destroys a move confirmation pop-up. */
	UFUNCTION(Category="User Interface")
	void CreateMoveConfirmationWidget(bool bDestroy, AParentPiece* PendingPiece);

	/* Updates the pending tile and piece of the displayed move confirmation widget. */
	UFUNCTION(Category="User Interface")
	void UpdateMoveConfirmationWidget(ABoardTile* PendingTile, AParentPiece* PendingPiece);

	/* Creates or destroys a move confirmation pop-up. */
	UFUNCTION(Category="User Interface")
	void CreateAttackConfirmationWidget(bool bDestroy, AParentPiece* FriendlyPiece);

	/* Updates the pending friendly and pending enemy pieces and the displayed information for the attack. */
	UFUNCTION(Category="User Interface")
	void UpdateAttackConfirmationWidget(AParentPiece* FriendlyPiece, AParentPiece* EnemyPiece);

	/* Creates or destroys an attack graphic. */
	UFUNCTION(BlueprintCallable, Category="User Interface")
	void UpdateAttackGraphicWidget(bool bDestroy, AParentPiece* Attacker, AParentPiece* Defender);

		/* Plays a given animation on the attack graphic. */
		UFUNCTION(BlueprintCallable, Category="User Interface")
		void PlayAttackGraphicAnimation(EAttackGraphicAnimation AttackGraphicAnim);


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


/* Protected components. */
protected:

	/* Handles communication between the server and the client. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components)
	UServerCommunicationComponent* ServerCommunicationComponent;


/* Protected widget class types. */
protected:

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> MatchSetup_SelectArmyClass;

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> MatchSetup_PlacePiecesClass;

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> Match_BaseWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> Match_PieceInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> Match_MoveConfirmationClass;

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> Match_AttackConfirmationClass;

	UPROPERTY(EditDefaultsOnly, Category="Widget Class Types")
	TSubclassOf<UUserWidget> Match_AttackGraphicClass;


/* Private widget objects (pointers to the above classes to access them after being created). */
private:

	/* Army selection widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatchSetup_SelectArmy* MatchSetup_SelectArmy = nullptr;

	/* Place pieces widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatchSetup_PlacePieces* MatchSetup_PlacePieces = nullptr;

	/* Base widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatch_BaseWidget* Match_BaseWidget = nullptr;

	/* Friendly piece info widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatch_PieceInfoWidget* Match_FriendlyPieceInfoWidget = nullptr;

	/* Enemy piece info widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatch_PieceInfoWidget* Match_EnemyPieceInfoWidget = nullptr;

	/* Move confirmation widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatch_MoveConfirmation* Match_MoveConfirmation = nullptr;

	/* Attack confirmation widget pointer. */
	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatch_AttackConfirmation* Match_AttackConfirmation = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UMatch_AttackGraphic* Match_AttackGraphic = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="Widget Objects")
	UUserWidget* ActiveAbilityConfirmationWidget = nullptr;

	/* Handles the match timer. */
	FTimerHandle MatchTimeTimerHandle;

};
