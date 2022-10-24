// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_AttackConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Components/RectLightComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Runtime/UMG/Public/UMG.h"

void UMatch_AttackConfirmation::UpdateAttackPreviewInfo(AParentPiece* FriendlyPiece, AParentPiece* EnemyPiece)
{
	/* Both given pieces need to be valid for this widget to update. */
	if (IsValid(FriendlyPiece) && IsValid(EnemyPiece))
	{
		/* Set references to this widget's pieces to use later. */
		PendingFriendlyPiece = FriendlyPiece;
		PendingEnemyPiece = EnemyPiece;

		/* If the piece data table was found... */
		if (PieceDataTable)
		{
			/* Get the friendly and enemy pieces' rows from the piece data table. */
			static const FString ContextString(TEXT("Piece Data Struct"));
			const FPieceDataStruct* FriendlyPieceData = PieceDataTable->FindRow<FPieceDataStruct>(FriendlyPiece->PieceID, ContextString, true);
			const FPieceDataStruct* EnemyPieceData = PieceDataTable->FindRow<FPieceDataStruct>(EnemyPiece->PieceID, ContextString, true);

			/* Update the displayed piece information using the piece data rows, if they could be retrieved. */
			if (FriendlyPieceData)
			{
				FriendlyImage->SetBrushFromTexture(FriendlyPieceData->AttackPreviewPhoto);
				FriendlyName->SetText(FText::FromString(FriendlyPieceData->PieceName.ToUpper()));
			}
			if (EnemyPieceData)
			{
				EnemyImage->SetBrushFromTexture(EnemyPieceData->AttackPreviewPhoto);
				EnemyName->SetText(FText::FromString(EnemyPieceData->PieceName.ToUpper()));
			}

			/* Update displayed piece information that can change during runtime using the piece's current stats. */
			FriendlyStrength->SetText(FText::FromString(FString::FromInt(FriendlyPiece->GetCurrentStrength())));
			FriendlyArmor->SetText(FText::FromString(FString::FromInt(FriendlyPiece->GetCurrentArmor())));
			EnemyStrength->SetText(FText::FromString(FString::FromInt(EnemyPiece->GetCurrentStrength())));
			EnemyArmor->SetText(FText::FromString(FString::FromInt(EnemyPiece->GetCurrentArmor())));
		}
	}
}

void UMatch_AttackConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the confirm attack button to execute the attack encounter. */
	AttackButton->OnClicked.AddDynamic(this, &UMatch_AttackConfirmation::OnAttackClicked);
	/* Bind the cancel button to reset the move action. */
	CancelButton->OnClicked.AddDynamic(this, &UMatch_AttackConfirmation::OnCancelClicked);
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

	/* Reset the highlight of every tile that was highlighted. */
	for (ABoardTile* Tile : PendingFriendlyPiece->GetValidMoveTiles())
	{
		Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
	}

	/* Remove all other widgets and reset the player's selections when starting the attack sequence. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true);

	/* Destroy this widget. */
	this->RemoveFromParent();
}

void UMatch_AttackConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->Server_SetPlayerStatus(E_SelectingPiece);
	/* Clear the player's selected piece. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->ClearSelection(true);

	if (IsValid(PendingFriendlyPiece))
	{
		/* Reset the highlight of every tile that was highlighted. */
		for (ABoardTile* Tile : PendingFriendlyPiece->GetValidMoveTiles())
		{
			Tile->UpdateEmissiveHighlight(false, 4.0f, Tile->EmissiveHighlight->GetLightColor());
		}
	}

	/* Destroy this widget. */
	RemoveFromParent();
}
