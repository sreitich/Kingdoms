// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_PieceInfoWidget.generated.h"

enum EAlignment;

class AParentPiece;

class UButton;
class UCanvasPanel;
class UDataTable;
class UImage;
class UOverlay;
class USizeBox;
class UTextBlock;
class UTexture2D;
class UVerticalBox;

struct FPieceDataStruct;

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
	bool UpdatePieceInfoWidget(AParentPiece* NewPiece, EAlignment Alignment, bool bEnableButtons);

	/* Allows the base widget to play this widget's animations. */
	UFUNCTION()
	void PlayOpenCloseAnim(bool bOpen, float StartTime, int32 NumLoops, EUMGSequencePlayMode::Type PlayMode, float Speed, bool bRestoreState);

	/* Refreshes this widget using all of the same parameters. */
	UFUNCTION()
	void RefreshWidget();

	/* Returns the currently displayed piece. */
	UFUNCTION()
	FORCEINLINE AParentPiece* GetDisplayedPiece() const { return DisplayedPiece; }

	/* Returns true if the player is currently hovering over this piece's stats. */
	UFUNCTION()
	bool AreStatsHovered() const;


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
	void EmptyBars(bool bReveal, bool bActive, int AmountUsed);


	/* Creates and displays a list of all current stat modifiers. */
	UFUNCTION()
	void OnStatsHovered();

	/* Calls BP_OnStatsUnhovered to destroy the list of current stat modifiers. */
	UFUNCTION()
	void OnStatsUnhovered();

		/* Destroys the list of current stat modifiers. */
		UFUNCTION(BlueprintImplementableEvent, Category="Modifiers")
		void BP_OnStatsUnhovered();

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


/* Public widgets. */
public:

	/* Pointer to the modifier list pop-up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMatch_ModifierList* ModifierList = nullptr;


/* Protected variables. */
protected:

	/* Enables binding functions to when these animations finish. */
	FWidgetAnimationDynamicEvent OpenFinishedDelegate;
	FWidgetAnimationDynamicEvent CloseFinishedDelegate;
	
	/* Pointer to the piece currently being displayed. */
	UPROPERTY()
	AParentPiece* DisplayedPiece;

	/* Pointer to the piece data table, used to retrieve information to display. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	UDataTable* PieceDataTable;

	/* Pointers to the images to use as backgrounds for each rarity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	UTexture2D* Background_Basic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	UTexture2D* Background_Superior;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	UTexture2D* Background_Heroic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Backgrounds")
	UTexture2D* Background_Champion;

	/* Pointers to the images that need to be changed depending on the affinity of the selected piece. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	UTexture2D* ActiveBackground_Friendly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	UTexture2D* ActiveBackground_Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	UTexture2D* PassiveBackground_Friendly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Affinity Indicators")
	UTexture2D* PassiveBackground_Enemy;

	/* The colors used for different text states. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Colors")
	FColor DefaultTextColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Colors")
	FColor BuffColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Colors")
	FColor DebuffColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Colors")
	FColor IneffectiveModifierColor;


/* Protected widgets. */
protected:

	/* The piece's photo background (dependent on rarity). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* DisplayedPhotoBackground;

	/* The piece's photo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* DisplayedPhoto;

	/* This piece's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedPieceName;

	/* This piece's rarity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedPieceRarity;

	/* This piece's class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedPieceClass;

	/* Contains the stats button layered on top of the strength and armor boxes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* StatsButtonOverlay;
	
	/* Displays a list of current stat modifiers when hovered over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* StatsButton;

	/* Widget that the modifier list pop-up is parented to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	USizeBox* ModifierListWrapper;

	/* This piece's current strength (including modifiers). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedStrength;

	/* This piece's current armor (including modifiers). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedArmor;

	/* Contains all of the active ability widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UVerticalBox* ActiveAbilityBox;

	/* Displays the active ability background and brings up a description when hovered over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ActiveAbilityBackgroundButton;

	/* Widget that the active ability pop-up is parented to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	USizeBox* ActiveAbilityPopupWrapper;

	/* Displays this piece's active ability's icon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* ActiveAbilityIcon;

	/* Semi-transparent image that greys out the active ability icon if the cooldown is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* ActiveCooldownIndicator;

	/* Live counter of how many turns before this piece's active ability can be used again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedActiveCD;

	/* Contains all of the passive ability widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UVerticalBox* PassiveAbilityBox;

	/* Displays the passive ability background and brings up a description when hovered over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* PassiveAbilityBackgroundButton;

	/* Widget that the passive ability pop-up is parented to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	USizeBox* PassiveAbilityPopupWrapper;

	/* Displays this piece's passive ability's icon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PassiveAbilityIcon;

	/* Semi-transparent image that greys out the passive ability icon if the cooldown is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PassiveCooldownIndicator;

	/* Live counter of how many turns before this piece's passive ability can be used again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayedPassiveCD;

	/* Button that prompts a piece to move or attack when clicked. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* MoveButton;

	/* Button that prompts a piece to use its active ability when clicked. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ActiveButton;

	/* Pointer to the ability info pop-up. */
	UPROPERTY()
	class UMatch_AbilityInfoPopup* AbilityInfoPopup = nullptr;


/* Protected widget class-types. */
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Class-Types")
	TSubclassOf<UMatch_ModifierList> ModifierListClass;
	


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
