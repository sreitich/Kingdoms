// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_AttackGraphic.h"

#include "MediaPlayer.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Runtime/UMG/Public/UMG.h"

#define INT_TO_FTEXT(x) FText::FromString(FString::FromInt(x))


void UMatch_AttackGraphic::UpdateAttackGraphicInfo(AParentPiece* Attacker, AParentPiece* Defender)
{
	if (IsValid(Attacker) && IsValid(Defender))
	{
		/* Ensure that the pointer to the piece data table was set correctly. */
		if (PieceDataTable)
		{
			/* Get the attacker and defender's information from the data table. */
			static const FString ContextString(TEXT("Piece Data Struct"));
			FPieceDataStruct* AttackerData = PieceDataTable->FindRow<FPieceDataStruct>(Attacker->GetPieceID(),
				ContextString, true);
			FPieceDataStruct* DefenderData = PieceDataTable->FindRow<FPieceDataStruct>(Defender->GetPieceID(),
				ContextString, true);

			/* Update the displayed attacker and defender information if they were found. Otherwise, there is a problem
			 * with the data table or the piece IDs. */
			if (AttackerData)
			{
				AttackerImage->SetBrushFromTexture(AttackerData->AttackPreviewPhoto);
				AttackerName->SetText(FText::FromString(AttackerData->PieceName));
			}
			if (DefenderData)
			{
				DefenderImage->SetBrushFromTexture(DefenderData->AttackPreviewPhoto);
				DefenderName->SetText(FText::FromString(DefenderData->PieceName));
			}

			/* Update displayed piece information that can change during runtime using the piece's current stats. */
			AttackerStrength->SetText(INT_TO_FTEXT(Attacker->GetCurrentStrength()));
			DefenderStrength->SetText(INT_TO_FTEXT(Defender->GetCurrentStrength()));
			AttackerArmor->SetText(INT_TO_FTEXT(Attacker->GetCurrentArmor()));
			DefenderArmor->SetText(INT_TO_FTEXT(Defender->GetCurrentArmor()));
		}
	}
}

void UMatch_AttackGraphic::PlayAttackGraphicAnimation(EAttackGraphicAnimation AttackGraphicAnim)
{
	switch (AttackGraphicAnim)
	{
	case E_NeutralFightInitiation:
		AnimationPlayer->OpenSource(NeutralFightInitiationAnim);
		break;
	case E_AttackerFightInitiation:
		AnimationPlayer->OpenSource(AttackerFightInitiationAnim);
		break;
	case E_AttackerDeath:
		AnimationPlayer->OpenSource(AttackerDeathAnim);
		break;
	case E_DefenderDeath:
		AnimationPlayer->OpenSource(DefenderDeathAnim);
		break;
	case E_AttackerDeadlock:
		AnimationPlayer->OpenSource(AttackerDeadlockAnim);
		break;
	case E_DefenderDeadlock:
		AnimationPlayer->OpenSource(DefenderDeadlockAnim);
		break;
	default:
		/* This should never happen. */
		break;
	}
}
