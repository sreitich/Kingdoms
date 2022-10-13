// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_BaseWidget.h"

#include "UserInterface/Match/Match_PieceInfoWidget.h"
#include "Pieces/ParentPiece.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Board/BoardTile.h"
#include "Kismet/GameplayStatics.h"
#include "UserInterface/Match/Match_MoveConfirmation.h"

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

void UMatch_BaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

}
