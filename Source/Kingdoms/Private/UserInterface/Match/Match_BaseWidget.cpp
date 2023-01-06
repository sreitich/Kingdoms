// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_BaseWidget.h"

#include "Framework/Match/Match_PlayerState.h"
#include "UserInterface/Match/Match_PieceInfoWidget.h"

#include "Runtime/UMG/Public/UMG.h"

#define OUT

void UMatch_BaseWidget::UpdateTimer_Implementation(int32 NewTime)
{
    /* Get the minutes as an integer. */
    FString Minutes = FString::FromInt(NewTime / 60);

    /* If the minutes are still in the single digits... */
    if (NewTime / 60 < 10)
    {
        /* Add a "0" before the minutes. */
        Minutes = "0" + Minutes;
    }

    /* Get the seconds as an integer. */
    FString Seconds = FString::FromInt(NewTime % 60);

    /* If the seconds are in the single digits... */
    if (NewTime % 60 < 10)
    {
        /* Add a "0" before the seconds. */
        Seconds = "0" + Seconds;
    }

    /* Update the displayed minutes text with the new Minutes string. */
    MatchTimeMinutes->SetText(FText::FromString(Minutes));

    /* Update the displayed seconds text with the new Seconds string. */
    MatchTimeSeconds->SetText(FText::FromString(Seconds));
}

void UMatch_BaseWidget::UpdateMapName_Implementation(FName NewName)
{
    /* Update the displayed map name with the passed FName. */
    MapName->SetText(FText::FromName(NewName));
}

UMatch_PieceInfoWidget* UMatch_BaseWidget::GetPieceInfoWidget(EAlignment Alignment) const
{
    if (Alignment == E_Friendly)
    {
        if (FriendlyPieceInfoWidget)
        {
            return FriendlyPieceInfoWidget;
        }
    }
    else if (Alignment == E_Hostile)
    {
        if (EnemyPieceInfoWidget)
        {
            return EnemyPieceInfoWidget;
        }
    }

    return nullptr;
}

void UMatch_BaseWidget::UpdateEndTurnButton(bool bEnable)
{
    /* Enable or disable the end turn button, unless it is already in the desired state. */
    if (bEnable != EndTurnButton->GetIsEnabled())
        EndTurnButton->SetIsEnabled(bEnable);
}

void UMatch_BaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    /* Bind the end turn button to end the player's turn when clicked. */
    EndTurnButton->OnClicked.AddDynamic(this, &UMatch_BaseWidget::OnEndTurnClicked);
}

void UMatch_BaseWidget::OnEndTurnClicked()
{
    if (AMatch_PlayerState* PlayerStatePtr = GetOwningPlayerState<AMatch_PlayerState>())
    {
        /* End this player's turn and start the next player's turn through the game state. */
        PlayerStatePtr->Server_EndPlayerTurn();

        /* Instantly update this player's status to reduce delay when refreshing piece info widgets. */
        PlayerStatePtr->SetPlayerStatus(E_WaitingForTurn);

        /* Disable this button. */
        EndTurnButton->SetIsEnabled(false);
    }
}
