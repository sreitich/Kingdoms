// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_AttackConfirmation.generated.h"

class AParentPiece;

class UButton;
class UDataTable;
class UImage;
class URichTextBlock;
class UTextBlock;
class UTexture2D;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_AttackConfirmation : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates the displayed attack information. */
	UFUNCTION()
	void UpdateAttackPreviewInfo(AParentPiece* FriendlyPiece, AParentPiece* EnemyPiece);

	/* Cleans up and destroys this widget. Resets the player and all tiles if bReset is true. */
	UFUNCTION()
	void DestroyWidget(bool bReset);


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

		/* Binds functions to the "Attack" and "Cancel" buttons in the NativeConstruct function. This allows us to bind
		 * different functions to the buttons for each child class by overriding this function, but not NativeConstruct. */
		virtual void BindButtons();

	/* Hides or reveals the displayed enemy's information and picture. Used when an attack confirmation widget does not
	 * have a target enemy piece selected. */
	UFUNCTION(BlueprintImplementableEvent)
	void SetEnemyInfoIsHidden(bool bHide);

	/* Updates the result preview text. Overrides need conditions for the pending enemy piece being valid and null.
	 * Default implementation assumes a two-way attack. */
	UFUNCTION()
	virtual void UpdateResultPreviewText() const;

	/* Attacks the enemy piece at the target tile. Can be overridden by custom attack confirmation widgets. */
	UFUNCTION()
	virtual void OnAttackClicked();

	/* Destroys this widget and resets the player and all tiles. */
	UFUNCTION()
	virtual void OnCancelClicked();


/* Protected variables. */
protected:
	
	/* References to the friendly and enemy pieces in this attack. */
	UPROPERTY()
	AParentPiece* PendingFriendlyPiece;

	UPROPERTY()
	AParentPiece* PendingEnemyPiece;


/* Protected assets. */
protected:

	/* Pointer to the piece data table, used to retrieve information to display. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	UDataTable* PieceDataTable;

	/* The preview image that displays when a target enemy piece has not been selected yet. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Assets")
	UTexture2D* UnknownEnemyImage;


/* Protected widgets. */
protected:

	/* Displayed pictures of the fighting pieces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* FriendlyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* EnemyImage;

	/* The piece's names. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* FriendlyName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* EnemyName;

	/* The friendly and enemy piece's stats. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* FriendlyStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
    UTextBlock* EnemyStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* FriendlyArmor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* EnemyArmor;
	
	/* The outcome of this fight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	URichTextBlock* DisplayedResult;

	/* Confirmation buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* AttackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;

};
