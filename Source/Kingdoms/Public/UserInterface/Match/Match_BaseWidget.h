// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_BaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_BaseWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called every frame. */
	/* virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override; */

	/* Updates the displayed minutes and seconds on the timer separately to maintain a constant alignment. */
	UFUNCTION(Client, Reliable)
	void UpdateTimer(int32 NewTime);

	/* Updates the displayed map name. */
	UFUNCTION(Client, Unreliable)
	void UpdateMapName(FName NewName);

	/* Returns a reference to the corresponding piece info widget. */
	UFUNCTION()
	class UMatch_PieceInfoWidget* GetPieceInfoWidget(bool bFriendly);


/* Protected functions. */
protected:

	/* Called after the underlying slate widget is constructed. */
	virtual void NativeConstruct() override;


/* Protected widgets. */
protected:

	/* Displays the minutes of the total match time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MatchTimeMinutes;

	/* Displays the seconds of the total match time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MatchTimeSeconds;

	/* Displays the current map being played on. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MapName;

	/* Displays info about the currently-selected friendly piece. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UMatch_PieceInfoWidget* FriendlyPieceInfoWidget;

	/* Displays info about the currently-selected enemy piece. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UMatch_PieceInfoWidget* EnemyPieceInfoWidget;

};
