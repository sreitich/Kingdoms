// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_QueueTimer.h"

#include "Components/Button.h"
#include "UserDefinedData/Match_UserDefinedData.h"

#include "Components/TextBlock.h"
#include "Framework/KingdomsGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UMM_QueueTimer::UpdateQueueTimer()
{
	/* Increment the current queue time. */
	CurrentQueueTime++;

	/* Set the text to the current queue time, converted to a minutes:seconds format. */
	TimerText->SetText(FText::FromString(FormatTime(CurrentQueueTime)));
}

void UMM_QueueTimer::FoundOpponent()
{
	/* Play a "found opponent" animation, displaying a new pop-up and disabling the "cancel" button. */
	PlayAnimationForward(FoundOpponentAnim, 1.0f, false);
}

void UMM_QueueTimer::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the "cancel" button to destroy the player's current session. */
	CancelButton->OnClicked.AddDynamic(this, &UMM_QueueTimer::OnCancelClicked);

	/* Reset the timer. */
	TimerText->SetText(FText::FromString("0:00"));

	/* Play the activation animation. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Call UpdateQueueTimer() every second. */
	GetWorld()->GetTimerManager().SetTimer(QueueTimerHandle, this, &UMM_QueueTimer::UpdateQueueTimer, 1.0f, true);
}

void UMM_QueueTimer::OnCancelClicked()
{
	// /* Leave the server while in the matchmaking state, so that it doesn't think that the player quit a match. */
	// if (UKingdomsGameInstance* GameInstancePtr = Cast<UKingdomsGameInstance>(UGameplayStatics::GetGameInstance(GetOwningPlayer())))
	// {
	// 	GameInstancePtr->LeaveServer(E_Matchmaking);
	// }

	BP_OnCancelClicked();
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
