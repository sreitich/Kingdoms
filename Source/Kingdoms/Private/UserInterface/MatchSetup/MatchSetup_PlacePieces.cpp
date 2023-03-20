// Copyright Change Studios, LLC 2023.


#include "UserInterface/MatchSetup/MatchSetup_PlacePieces.h"

#include "Framework/Match/Match_PlayerState.h"
#include "UserDefinedData/SaveGameData_UserDefinedData.h"
#include "UserInterface/MatchSetup/MatchSetup_UnplacedPieceWidget.h"

#include "Runtime/UMG/Public/UMG.h"

void UMatchSetup_PlacePieces::LoadArmy(FArmyPresetStruct ArmyToLoad)
{
    /* Add every piece in the army to the list of unplaced pieces. For every piece in the army... */
	for (int i = 0; i < ArmyToLoad.Pieces.Num(); i++)
	{
		/* If the unplaced piece widget class has been set... */
		if (IsValid(MatchSetup_UnplacedPieceWidgetClass))
		{
			/* Create an unplaced piece widget for this piece. The widget's name is the piece's ID followed by its index in the army. */
			UMatchSetup_UnplacedPieceWidget* UnplacedPieceWidget = CreateWidget<UMatchSetup_UnplacedPieceWidget>(GetOwningPlayer(), MatchSetup_UnplacedPieceWidgetClass, FName(ArmyToLoad.Pieces[i] + " - " + FString::FromInt(i)));

			/* If the widget was successfully created... */
			if (UnplacedPieceWidget)
			{
				/* Add the unplaced piece widget to the unplaced piece list container. */
				PiecesBox->AddChild(UnplacedPieceWidget);

				/* Tell the unplaced piece widget which piece it represents and update its information. */
				UnplacedPieceWidget->UpdateUnplacedPieceWidget(ArmyToLoad.Pieces[i]);

				/* Add padding between each widget. */
				UnplacedPieceWidget->SetPadding(FMargin(15.0f, 0.0f));
			}
		}
	}
}

void UMatchSetup_PlacePieces::PlayRevealCardsAnim(bool bHide)
{
	/* Hide all cards if requested. */
	if (bHide)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);

		PlayAnimationForward
		(
			HideCardsAnim,
			1.0f,
			false
		);
	}
	/* Reveal all cards if requested. */
	else
	{
		SetVisibility(ESlateVisibility::Visible);

		PlayAnimationReverse
		(
			HideCardsAnim,
			1.0f,
			false
		);
	}
}

void UMatchSetup_PlacePieces::CheckAllPiecesPlaced() const
{
	/* If this player has no more unplaced pieces... */
	if (PiecesBox->GetChildrenCount() == 0)
	{
		/* Allow them to declare that they are ready. */
		ReadyButton->SetIsEnabled(true);
	}
}

void UMatchSetup_PlacePieces::NativeConstruct()
{
	Super::NativeConstruct();

    /* Bind the ready button to attempt to start the match if both players have clicked it. */
    ReadyButton->OnClicked.AddDynamic(this, &UMatchSetup_PlacePieces::OnReadyClicked);
}

void UMatchSetup_PlacePieces::OnReadyClicked()
{
    /* Ask the server to set this player's bReadyToPlay variable to true. */
    GetOwningPlayerState<AMatch_PlayerState>(false)->SetReadyToPlay(true);

	/* Make the ready button green and disable it. */
	ReadyButton->SetBackgroundColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
	ReadyButton->SetIsEnabled(false);
}
