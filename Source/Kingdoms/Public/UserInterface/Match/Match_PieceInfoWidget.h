// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_PieceInfoWidget.generated.h"

struct FPieceDataStruct;

class AParentPiece;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_PieceInfoWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates all displayed piece information by retrieving data from the piece data table. Returns whether any
	 * displayed information changed if the widget was already open. */
	UFUNCTION()
	bool UpdatePieceInfoWidget(AParentPiece* NewPiece, bool bIsFriendly, bool bEnableButtons);

	/* Allows the base widget to play this widget's animations. */
	UFUNCTION()
	void PlayOpenCloseAnim(bool bOpen, float StartTime, int32 NumLoops, EUMGSequencePlayMode::Type PlayMode, float Speed, bool bRestoreState);


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Called when the opening animation finishes playing, enabling it. */
	UFUNCTION()
	void OpeningAnimFinished();

	/* Called when the closing animation finishes playing, hiding it. */
	UFUNCTION()
	void ClosingAnimFinished();

	/* Removes all uses bars and adds the passed amount. bActive determines which ability's bars to target. */
	UFUNCTION(BlueprintImplementableEvent, Category="Updating Piece Info Widget")
	void ResetBars(bool bActive, int NewAmount);

	/* Incrementally sets the passed number of uses bars to be empty (used). bActive determines which ability's bars to target. */
	UFUNCTION(BlueprintImplementableEvent, Category="Updating Piece Info Widget")
	void EmptyBars(bool bActive, int AmountUsed);

	/* Displays and updates ability info pop-up with this piece's active ability info. */
	UFUNCTION()
	void OnActiveHovered();

	/* Displays and updates ability info pop-up with this piece's passive ability info. */
	UFUNCTION()
	void OnPassiveHovered();

	/* Destroys the ability info pop-up. */
	UFUNCTION()
	void OnAbilityUnhovered();

	/* Begins targeting a location to move to or to attack. */
	UFUNCTION()
	void OnMoveClicked();

	/* Call's the piece's implemented ability function. */
	UFUNCTION()
	void OnUseActiveClicked();


/* Protected variables. */
protected:

	/* Enables binding functions to when these animations finish. */
	FWidgetAnimationDynamicEvent OpenFinishedDelegate;
	FWidgetAnimationDynamicEvent CloseFinishedDelegate;
	
	/* Pointer to the piece currently being displayed. */
	UPROPERTY()
	class AParentPiece* DisplayedPiece;

	/* Pointer to the piece data table, used to retrieve information to display. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	class UDataTable* PieceDataTable;

	/* Pointers to the images to use as backgrounds for each rarity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	class UTexture2D* Background_Basic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	class UTexture2D* Background_Superior;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	class UTexture2D* Background_Heroic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	class UTexture2D* Background_Champion;

	/* Pointers to the images that need to be changed depending on the affinity of the selected piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	class UTexture2D* ActiveBackground_Friendly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	class UTexture2D* ActiveBackground_Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	class UTexture2D* PassiveBackground_Friendly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	class UTexture2D* PassiveBackground_Enemy;


/* Protected widget class types. */
protected:

	/* Class reference for spawning the ability info pop-up when an ability is hovered over. */
	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UUserWidget> AbilityInfoClass;


/* Protected widgets. */
protected:

	/* The piece's photo background (dependent on rarity). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* DisplayedPhotoBackground;

	/* The piece's photo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* DisplayedPhoto;

	/* This piece's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedPieceName;

	/* This piece's rarity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedPieceRarity;

	/* This piece's class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedPieceClass;

	/* This piece's current strength (including modifiers). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedStrength;

	/* This piece's current armor (including modifiers). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedArmor;

	/* Contains all of the active ability widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* ActiveAbilityBox;

	/* Displays the active ability background and brings up a description when hovered over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* ActiveAbilityBackgroundButton;

	/* Displays this piece's active ability's icon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* ActiveAbilityIcon;

	/* Semi-transparent image that greys out the active ability icon if the cooldown is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* ActiveCooldownIndicator;

	/* Live counter of how many turns before this piece's active ability can be used again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedActiveCD;

	/* Contains all of the passive ability widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* PassiveAbilityBox;

	/* Displays the passive ability background and brings up a description when hovered over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* PassiveAbilityBackgroundButton;

	/* Displays this piece's passive ability's icon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* PassiveAbilityIcon;

	/* Semi-transparent image that greys out the passive ability icon if the cooldown is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* PassiveCooldownIndicator;

	/* Live counter of how many turns before this piece's passive ability can be used again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DisplayedPassiveCD;

	/* Button that prompts a piece to move or attack when clicked. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* MoveButton;

	/* Button that prompts a piece to use its active ability when clicked. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* ActiveButton;

	/* Pointer to the ability info pop-up to manipulate it. */
	UPROPERTY()
	class UMatch_AbilityInfo* AbilityInfoWidget = nullptr;


/* Protected animations. */
protected:

	/* Plays when revealing widget. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* PieceInfoOpenAnim;

	/* Plays when hiding widget. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* PieceInfoCloseAnim;


/* Private variables. */
private:

	/* Pointer to the currently selected piece's data in the piece data table. */
	FPieceDataStruct* PieceData;

};
