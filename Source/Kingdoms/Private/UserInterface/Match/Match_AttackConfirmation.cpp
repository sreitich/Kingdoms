// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_AttackConfirmation.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
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
			FPieceDataStruct* FriendlyPieceData = PieceDataTable->FindRow<FPieceDataStruct>(FriendlyPiece->GetPieceID(), ContextString, true);
			FPieceDataStruct* EnemyPieceData = PieceDataTable->FindRow<FPieceDataStruct>(EnemyPiece->GetPieceID(), ContextString, true);

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
	AttackInfo.bDefenderFights = true;
	AttackInfo.bMoveTo = true;
	AttackInfo.bTakePlace = true;
	
	/* Tell the server to execute the attack logic with authority and on each client. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->Server_Attack(AttackInfo);

	/* Refresh each tile's highlight to clear the highlights for attacking options. */
	TArray<AActor*> AllTiles;
	for (AActor* Tile : AllTiles)
	{
		Cast<ABoardTile>(Tile)->RefreshHighlight();
	}

	/* Destroy this widget. */
	this->RemoveFromParent();
}

void UMatch_AttackConfirmation::OnCancelClicked()
{
	/* Reset the player state. */
	GetOwningPlayerPawn<AMatch_PlayerPawn>()->GetPlayerState<AMatch_PlayerState>()->SetPlayerStatus_Server(E_SelectingPiece);

	/* For every tile that was highlighted... */
	for (ABoardTile* Tile : PendingFriendlyPiece->GetValidTiles())
	{
		/* Refresh the tile's highlight depending on its occupying piece to clear the highlights. */
		Tile->RefreshHighlight();
	}

	/* Destroy this widget. */
	RemoveFromParent();
}
