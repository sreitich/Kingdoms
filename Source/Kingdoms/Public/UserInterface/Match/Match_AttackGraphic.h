// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_AttackGraphic.generated.h"

enum EAttackGraphicAnimation;

class AParentPiece;
class UDataTable;
class UImage;
class UMediaPlayer;
class UMediaSource;
class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_AttackGraphic : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	UFUNCTION()
	void UpdateAttackGraphicInfo(AParentPiece* Attacker, AParentPiece* Defender);
	
	/* Plays a given animation on the widget. */
	UFUNCTION()
	void PlayAttackGraphicAnimation(EAttackGraphicAnimation AttackGraphicAnim) const;


/* Protected assets. */
protected:

	/* Pointer to the piece data table, used to retrieve information to display. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	const UDataTable* PieceDataTable;
	
	/* Used to play animations on each part of the screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Media")
	UMediaPlayer* AttackerAnimPlayer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Media")
	UMediaPlayer* DefenderAnimPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Media")
	UMediaPlayer* CenterAnimPlayer;

	/* Video files to play on the attack graphic. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UMediaSource* NeutralFightInitiationAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UMediaSource* AttackerFightInitiationAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UMediaSource* AttackerDeathAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UMediaSource* DefenderDeathAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UMediaSource* AttackerDeadlockAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UMediaSource* DefenderDeadlockAnim;


/* Protected widgets. */
protected:

	/* These images are set to an animated texture when an animation needs to be played over the attacker's picture,
	 * over the defender's picture, or in the center of the graphic, respectively. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* AttackerAnimationImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* DefenderAnimationImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* CenterAnimationImage;

	/* The attacker's displayed picture and information. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* AttackerImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* AttackerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* AttackerStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* AttackerArmor;

	/* The defender's displayed picture and information. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* DefenderImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DefenderName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DefenderStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DefenderArmor;
	
};
