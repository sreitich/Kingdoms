// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_QueueTimer.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_QueueTimer : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Called every second to update the displayed time. */
	UFUNCTION()
	void UpdateQueueTimer();

	/* Called when an opponent is found, preventing the player from "cancelling" matchmaking until they enter their
	 * game. */
	UFUNCTION()
	void FoundOpponent();


/* Protected functions. */
protected:

	/* Called when this widget is created. */
	virtual void NativeConstruct() override;

	/* Called when the "cancel" button is clicked to leave and destroy the current session. */
	UFUNCTION()
	void OnCancelClicked();


/* Protected widgets. */
protected:

	/* The text displaying the current timer. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TimerText;

	/* Leaves and destroys the current session. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;


/* Protected animations. */
protected:

	/* Plays when this widget is spawned. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnActivatedAnim;

	/* Plays when an opponent is found and the queue ends. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FoundOpponentAnim;


/* Private functions. */
private:

	/* Converts the given time to a minutes:seconds format. */
	FString FormatTime(int TotalSeconds) const;


/* Private variables. */
private:

	/* The amount of time since the player started their queue, in seconds. */
	UPROPERTY()
	int CurrentQueueTime;

	/* Handles the timer. */
	UPROPERTY()
	FTimerHandle QueueTimerHandle;
	
};
