// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_AttackConfirmation.h"

#include "Board/BoardTile.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Components/Button.h"
#include "Components/RectLightComponent.h"
#include "Components/RichTextBlock.h"
#include "Framework/Match/Match_PlayerController.h"
#include "Runtime/UMG/Public/UMG.h"

void UMatch_AttackConfirmation::UpdateAttackPreviewInfo(AParentPiece* FriendlyPiece, AParentPiece* EnemyPiece)
{
	/* Used to retrieve data from the piece data table. */
	static const FString ContextString(TEXT("Piece Data Struct"));
	
	/* If the piece data table was found... */
	if (PieceDataTable)
	{
		/* Update the pending friendly piece and the displayed information if the given friendly piece is valid. */
		if (IsValid(FriendlyPiece))
		{
			/* Set a reference to this widget's pending friendly piece to use when confirming the attack. */
			PendingFriendlyPiece = FriendlyPiece;

			/* Get the friendly piece's row from the data table. */
			const FPieceDataStruct* FriendlyPieceData = PieceDataTable->FindRow<FPieceDataStruct>(FriendlyPiece->PieceID, ContextString, true);

			/* Update the displayed friendly piece information that doesn't change during the game, if the row was found. */
			if (FriendlyPiece)
			{
				FriendlyImage->SetBrushFromTexture(FriendlyPieceData->AttackPreviewPhoto);
				FriendlyName->SetText(FText::FromString(FriendlyPieceData->PieceName.ToUpper()));
			}

			/* Update the displayed friendly piece information that can change during the game using the piece's current stats. */
			FriendlyStrength->SetText(FText::FromString(FString::FromInt(FriendlyPiece->GetCurrentStrength())));
			FriendlyArmor->SetText(FText::FromString(FString::FromInt(FriendlyPiece->GetCurrentArmor())));
		}

		/* Update the pending enemy piece, reveal and update its displayed information, and enable the attack confirmation if the given enemy piece is valid. */
		if (IsValid(EnemyPiece))
		{
			/* Set a reference to this widget's pending enemy piece to use when confirming the attack. */
			PendingEnemyPiece = EnemyPiece;

			/* Get the enemy piece's row from the data table. */
			const FPieceDataStruct* EnemyPieceData = PieceDataTable->FindRow<FPieceDataStruct>(EnemyPiece->PieceID, ContextString, true);

			/* Update the displayed enemy piece information that doesn't change during the game, if the row was found. */
			if (EnemyPieceData)
			{
				EnemyImage->SetBrushFromTexture(EnemyPieceData->AttackPreviewPhoto);
				EnemyName->SetText(FText::FromString(EnemyPieceData->PieceName.ToUpper()));
			}

			/* Update the displayed enemy piece information that can change during the game using the piece's current stats. */
			EnemyStrength->SetText(FText::FromString(FString::FromInt(EnemyPiece->GetCurrentStrength())));
			EnemyArmor->SetText(FText::FromString(FString::FromInt(EnemyPiece->GetCurrentArmor())));

			/* Enable the widget's "confirm" button. */
			AttackButton->SetIsEnabled(true);

			/* Reveal the pending enemy piece's information and picture. */
			SetEnemyInfoIsHidden(false);
		}
		/* Hide the enemy piece's displayed information and disable the confirmation button if the given enemy piece is null. */
		else
		{
			/* Might need to reset the pending piece. */
			// PendingEnemyPiece = nullptr;
			
			/* Disable the widget's "confirm" button. */
			AttackButton->SetIsEnabled(false);

			/* Hide the unknown enemy's information and update its preview image to show an unknown piece. */
			SetEnemyInfoIsHidden(true);
		}
	}

	/* Update the displayed result text. */
	UpdateResultPreviewText();
}

void UMatch_AttackConfirmation::DestroyWidget(bool bReset)
{
	AMatch_PlayerPawn* PlayerPawnPtr = GetOwningPlayerPawn<AMatch_PlayerPawn>();

	/* Reset the player state and highlighted tiles if requested. */
	if (bReset)
	{
		/* Reset the player state. */
		PlayerPawnPtr->GetPlayerState<AMatch_PlayerState>()->SetPlayerStatus(E_SelectingPiece);

		/* Reset the highlight of every tile that was highlighted. */
		if (IsValid(PendingFriendlyPiece))
		{
			for (ABoardTile* Tile : PendingFriendlyPiece->GetValidMoveTiles())
			{
				Tile->RemoveTargetHighlight();
			}
		}

		/* Clear the player's selected piece and selected enemy piece and hide the piece info widgets. */
		PlayerPawnPtr->ClearSelection(true, true, false, true, true);
	}
    /* Just reset the player's selected tile if bReset is false. This is typically only used when switching to a move widget. */
	else
	{
		PlayerPawnPtr->ClearSelection(false, true, false, true, false);
	}

	/* Clear this widget's information and disable the confirmation button. */
	PendingFriendlyPiece = nullptr;
	PendingEnemyPiece = nullptr;
	AttackButton->SetIsEnabled(false);

	/* Destroy this widget. */
	RemoveFromParent();
}

void UMatch_AttackConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the correct functions to the "Attack" and "Cancel" buttons. The functions that these buttons are bound to
	 * can be overridden in each child class. */
	BindButtons();
}

void UMatch_AttackConfirmation::BindButtons()
{
	/* Bind the confirm attack button to execute the attack encounter. */
	AttackButton->OnClicked.AddDynamic(this, &UMatch_AttackConfirmation::OnAttackClicked);
	/* Bind the cancel button to reset the move action. */
	CancelButton->OnClicked.AddDynamic(this, &UMatch_AttackConfirmation::OnCancelClicked);
}

void UMatch_AttackConfirmation::UpdateResultPreviewText() const
{
	/* Update the text depending on the result if both pieces are known. */
	if (IsValid(PendingFriendlyPiece) && IsValid(PendingEnemyPiece))
	{
		/* Save the piece's stats for readability. */
		const int AttStr = PendingFriendlyPiece->GetCurrentStrength();
		const int AttArm = PendingFriendlyPiece->GetCurrentArmor();
		const int DefStr = PendingEnemyPiece->GetCurrentStrength();
		const int DefArm = PendingEnemyPiece->GetCurrentArmor();

		/* Attacker defeats defender. */
		if (AttStr >= DefArm && DefStr < AttArm)
		{
			DisplayedResult->SetText(FText::FromString("<WinningResult>Attacker will defeat defender.</>"));
		}
		/* Defender defeats attacker. */
		else if (DefStr >= AttArm && AttStr < DefArm)
		{
			DisplayedResult->SetText(FText::FromString("<LosingResult>Defender will defeat attacker.</>"));
		}
		/* Attack and defender kill each other. */
		else if (AttStr >= DefArm && DefStr >= AttArm)
		{
			DisplayedResult->SetText(FText::FromString("<EvenResult>Attacker and defender will both die.</>"));
		}
		/* Attacker and defender both survive. */
		else if (DefArm > AttStr && AttArm > DefStr)
		{
			DisplayedResult->SetText(FText::FromString("<EvenResult>Attacker and defender will both survive.</>"));
		}
	}
	/* If the enemy piece is unknown, prompt the user to select a target. */
	else if (!IsValid(PendingEnemyPiece))
	{
		DisplayedResult->SetText(FText::FromString("Selecting a target..."));
	}
}

void UMatch_AttackConfirmation::OnAttackClicked()
{
	/* Parameters used for an attack from a move. */
	FAttackInfo AttackInfo;
	AttackInfo.Attacker = PendingFriendlyPiece;
	AttackInfo.Defender = PendingEnemyPiece;
	AttackInfo.bMoveTo = true;
	AttackInfo.bMoved = false;
	AttackInfo.bDefenderFights = true;
	AttackInfo.bTakePlace = true;
	
	/* Tell the server to execute the attack logic with authority and on each client. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->Server_Attack(AttackInfo, true);

	/* Record that the player has used their move action for this turn, preventing them from using another move action until their next turn. */
	GetOwningPlayerState<AMatch_PlayerState>()->SetMoveActionUsed();

	/* Destroy this widget through the player controller to clean up references. */
	if (AMatch_PlayerController* PlayerControllerPtr = Cast<AMatch_PlayerController>(GetOwningPlayer()))
		PlayerControllerPtr->CreateAttackConfirmationWidget(true, nullptr);
}

void UMatch_AttackConfirmation::OnCancelClicked()
{
	/* Destroy this widget through the player controller to clean up references. */
	if (AMatch_PlayerController* PlayerControllerPtr = Cast<AMatch_PlayerController>(GetOwningPlayer()))
		PlayerControllerPtr->CreateAttackConfirmationWidget(true, nullptr);
}
