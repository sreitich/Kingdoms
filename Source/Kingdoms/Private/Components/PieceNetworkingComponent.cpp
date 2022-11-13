// Copyright Samuel Reitich 2023.


#include "Components/PieceNetworkingComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"

#include "Pieces/ParentPiece.h"

UPieceNetworkingComponent::UPieceNetworkingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UPieceNetworkingComponent::Server_AddModifier_Implementation(AParentPiece* PieceToModify, FModifier NewModifier,
	bool bActivatePopUp, bool bFlashHighlight)
{
	/* Store the original strength or armor value to find out the final change that this modifier applies. */
	const int OriginalValue = NewModifier.EffectedStat ? PieceToModify->GetCurrentStrength() : PieceToModify->GetCurrentArmor();
	int NewValue = OriginalValue;

	/* Save the new strength value to determine the modifier pop-up value. */
	if (NewModifier.StrengthChange != 0)
		NewValue = FMath::Clamp(PieceToModify->GetCurrentStrength() + NewModifier.StrengthChange, 0, 20);

	/* Save the new armor value to determine the modifier pop-up value. */
	if (NewModifier.ArmorChange != 0)
		NewValue = FMath::Clamp(PieceToModify->GetCurrentArmor() + NewModifier.ArmorChange, 0, 20);

	/* Get the target piece's current temporary modifiers. */
	TArray<FModifier> &TemporaryModifiers = PieceToModify->GetTemporaryModifiers();
	
	/* Check if this modifier is already applied. */
	int RepeatIndex = -1;
	for (int i = 0; i < TemporaryModifiers.Num(); i++)
	{
		if (TemporaryModifiers[i].SourceActor == NewModifier.SourceActor &&
			TemporaryModifiers[i].SourceAbilityName == NewModifier.SourceAbilityName &&
			TemporaryModifiers[i].StrengthChange == NewModifier.StrengthChange &&
			TemporaryModifiers[i].ArmorChange == NewModifier.ArmorChange)
		{
			RepeatIndex = i;
			break;
		}
	}

	/* If this modifier is already applied, reset its duration and stack the values together. */
	if (RepeatIndex != -1)
	{
		TemporaryModifiers[RepeatIndex].RemainingDuration = NewModifier.RemainingDuration;
		TemporaryModifiers[RepeatIndex].StrengthChange += NewModifier.StrengthChange;
		TemporaryModifiers[RepeatIndex].ArmorChange += NewModifier.ArmorChange;
	}
	/* If this modifier isn't already applied, apply it. */
	else
	{
		TArray<FModifier> OldTemporaryModifiers = TemporaryModifiers;
		TemporaryModifiers.Add(NewModifier);
		/* Manually call the OnRep for the server. */
		PieceToModify->OnRep_TemporaryModifiers(OldTemporaryModifiers);
	}

	/* Spawn a modifier pop-up if requested. */
	if (bActivatePopUp)
		PieceToModify->Multicast_CreateModifierPopUp(NewValue - OriginalValue, NewModifier.EffectedStat == FModifier::Strength);

	/* Flash a piece highlight for each changed statistic on all clients if requested. */
	if (bFlashHighlight)
	{
		if (NewModifier.StrengthChange != 0)
		{
			if (AMatch_PlayerPawn* PawnPtr = Cast<AMatch_PlayerPawn>(GetOuter()))
			{
				PawnPtr->Multicast_FlashHighlight
				(
					PieceToModify,
					NewModifier.StrengthChange > 0 ? FLinearColor(0.0f, 1.0f, 0.0f) : FLinearColor(1.0f, 0.0f, 0.0f),
					10.0f,
					0.5f,
					0.25f,
					false
				);
			}
		}

		if (NewModifier.ArmorChange != 0)
		{
			if (AMatch_PlayerPawn* PawnPtr = Cast<AMatch_PlayerPawn>(GetOuter()))
			{
				PawnPtr->Multicast_FlashHighlight
				(
					PieceToModify,
					NewModifier.ArmorChange > 0 ? FLinearColor(0.0f, 1.0f, 0.0f) : FLinearColor(1.0f, 0.0f, 0.0f),
					10.0f,
					0.5f,
					0.25f,
					false
				);
			}
		}
	}
}

// void UPieceNetworkingComponent::Server_RemoveModifier_Implementation(AParentPiece* TargetPiece, FModifier ModifierToRemove, bool bActivatePopUp,
// 	bool bFlashHighlight)
// {
// 	/* Some pieces' abilities that have lasting effects (i.e. modifiers) need to execute code when that effect ends. */
// 	const TArray<AActor*> Targets = { TargetPiece };
// 	if (AParentPiece* Piece = Cast<AParentPiece>(ModifierToRemove.SourceActor))
// 		Piece->OnAbilityEffectEnded(Targets);
//
// 	/* Remove the strength effect of the modifier from the piece it was modifying. */
// 	if (ModifierToRemove.StrengthChange != 0)
// 	{
// 		CurrentStrength = FMath::Clamp(CurrentStrength - ModifierToRemove.StrengthChange, 0, 20);
// 		OnRep_CurrentStrength();
// 	}
//
// 	/* Remove the armor effect of the modifier from the piece it was modifying. */
// 	if (ModifierToRemove.ArmorChange != 0)
// 	{
// 		CurrentArmor = FMath::Clamp(CurrentArmor - ModifierToRemove.ArmorChange, 0, 20);
// 		OnRep_CurrentArmor();
// 	}
// 			
// 	/* Remove this modifier from this piece. */
// 	TemporaryModifiers.Remove(ModifierToRemove);
// }

void UPieceNetworkingComponent::BeginPlay()
{
	Super::BeginPlay();
}
