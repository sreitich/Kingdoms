// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_MoveConfirmation.generated.h"

class ABoardTile;
class AParentPiece;

class UButton;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_MoveConfirmation : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Getter for PendingTile. */
	UFUNCTION(BlueprintPure, Category="Pending Tile")
	FORCEINLINE ABoardTile* GetPendingTile() const { return PendingTile; }

	/* Setter for PendingTile. */
	UFUNCTION(BlueprintCallable, Category="Pending Tile")
	bool SetPendingTile(ABoardTile* NewPendingTile);

	/* Getter for PendingPiece. */
	UFUNCTION(BlueprintPure, Category="Pending Piece")
	FORCEINLINE AParentPiece* GetPendingPiece() const { return PendingPiece; }

	/* Setter for PendingPiece. */
	UFUNCTION(BlueprintCallable, Category="Pending Piece")
	bool SetPendingPiece(AParentPiece* NewPendingPiece);

	/* Enables/disables for the confirmation button. */
	UFUNCTION(BlueprintCallable, Category="Confirm Button")
	void SetConfirmButtonIsEnabled(bool bConfirmIsEnabled);

	/* Cleans up and destroys this widget. Resets the player and all tiles if bReset is true. */
	UFUNCTION()
	void DestroyWidget(bool bReset);


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Moves the selected piece to the selected tile. */
	UFUNCTION()
	void OnConfirmClicked();

	/* Destroys this widget and resets the player and all tiles. */
	UFUNCTION()
	void OnCancelClicked();


/* Protected variables. */
protected:

	/* Pointer to the tile that the player is confirming a move to. */
	UPROPERTY()
	ABoardTile* PendingTile;

	/* Pointer to the piece that the player is confirming to move. */
	UPROPERTY()
	AParentPiece* PendingPiece;


/* Protected widgets. */
protected:

	/* Button that confirms the player's move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ConfirmButton;

	/* Button that cancels the player's move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;

};
