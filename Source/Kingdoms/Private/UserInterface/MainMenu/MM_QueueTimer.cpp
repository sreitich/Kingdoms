// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_QueueTimer.h"

#include "Components/TextBlock.h"

void UMM_QueueTimer::UpdateQueueTimer()
{
	/* Increment the current queue time. */
	CurrentQueueTime++;

	/* Set the text to the current queue time, converted to a minutes:seconds format. */
	TimerText->SetText(FText::FromString(FormatTime(CurrentQueueTime)));
}

void UMM_QueueTimer::NativeConstruct()
{
	Super::NativeConstruct();

	/* Play the activation animation. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Call UpdateQueueTimer() every second. */
	GetWorld()->GetTimerManager().SetTimer(QueueTimerHandle, this, &UMM_QueueTimer::UpdateQueueTimer, 1.0f, true);
}

FString UMM_QueueTimer::FormatTime(int TotalSeconds) const
{
	/* Convert the given seconds into a minutes:seconds format. */
	const int Minutes = TotalSeconds / 60;
	const int Seconds = TotalSeconds % 60;

	FString MinutesStr = FString::FromInt(Minutes);
	FString SecondsStr = FString::FromInt(Seconds);

	if (Minutes && Minutes < 10)
		MinutesStr = "0" + MinutesStr;
	if (Seconds < 10)
		SecondsStr = "0" + SecondsStr;

	FString FormattedTime = FString::Printf(TEXT("%s:%s"), *MinutesStr, *SecondsStr);
	return FormattedTime;
}