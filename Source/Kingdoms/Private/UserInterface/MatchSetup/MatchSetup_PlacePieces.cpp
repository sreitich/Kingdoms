// Copyright Samuel Reitich 2022.


#include "UserInterface/MatchSetup/MatchSetup_PlacePieces.h"

#include "Framework/Match/Match_PlayerState.h"
#include "UserDefinedData/SaveGameData_UserDefinedData.h"
#include "UserInterface/MatchSetup/MatchSetup_UnplacedPieceWidget.h"

#include "Runtime/UMG/Public/UMG.h"

void UMatchSetup_PlacePieces::LoadArmy(FArmyPresetStruct ArmyToLoad)
{
    /* For every piece in the army... */
	for (int ii = 0; ii < ArmyToLoad.Pieces.Num(); ii++)
	{
		/* If the unplaced piece widget class has been set... */
		if (IsValid(MatchSetup_UnplacedPieceWidgetClass))
		{
			/* Create an unplaced piece widget for this piece. The widget's name is the piece's ID followed by its index in the army. */
			UMatchSetup_UnplacedPieceWidget* UnplacedPieceWidget = CreateWidget<UMatchSetup_UnplacedPieceWidget>(this, MatchSetup_UnplacedPieceWidgetClass, FName(ArmyToLoad.Pieces[ii] + " - " + FString::FromInt(ii)));

			/* If the widget was successfully created... */
			if (UnplacedPieceWidget)
			{
				/* Add the unplaced piece widget to the unplaced piece list container. */
				PiecesBox->AddChild(UnplacedPieceWidget);

				/* Tell the unplaced piece widget which piece it represents and update its information. */
				UnplacedPieceWidget->UpdateUnplacedPieceWidget(ArmyToLoad.Pieces[ii]);

				/* Add padding between each widget. */
				UnplacedPieceWidget->SetPadding(FMargin(15.0f, 0.0f));
			}
		}
	}
}

void UMatchSetup_PlacePieces::PlayHideCardsAnim(bool bReverse)
{
	/* Play the HideCards animation to hide all remaining cards off-screen. */
	PlayAnimation
	(
		HideCardsAnim,
		0.0f,
		1,
		bReverse ? EUMGSequencePlayMode::Reverse : EUMGSequencePlayMode::Forward,
		1.0f,
		false
	);
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
    GetOwningPlayerState<AMatch_PlayerState>(false)->SetReadyToPlay_Server(true);

	/* Make the ready button green. */
	ReadyButton->SetBackgroundColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
}
