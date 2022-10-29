// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_AttackConfirmation.generated.h"

class AParentPiece;

class UImage;
class UTextBlock;
class UButton;

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

	/* Resets the move action. Public to be used by the player controller when deselecting a piece while selecting a
	 * move destination. */
	UFUNCTION()
	virtual void OnCancelClicked();


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

		/* Binds functions to the "Attack" and "Cancel" buttons in the NativeConstruct function. This allows us to bind
		 * different functions to the buttons for each child class by overriding this function, but not NativeConstruct. */
		virtual void BindButtons();

	/* Attacks the enemy piece at the target tile. Can be overridden by custom attack confirmation widgets. */
	UFUNCTION()
	virtual void OnAttackClicked();


/* Protected variables. */
protected:

	/* Pointer to the piece data table, used to retrieve information to display. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	class UDataTable* PieceDataTable;
	
	/* References to the friendly and enemy pieces in this attack. */
	UPROPERTY()
	AParentPiece* PendingFriendlyPiece;

	UPROPERTY()
	AParentPiece* PendingEnemyPiece;


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
	UTextBlock* DisplayedResult;

	/* Confirmation buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* AttackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;

};
