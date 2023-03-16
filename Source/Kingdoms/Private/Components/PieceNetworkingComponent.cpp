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
	bool bActivatePopUps)
{
	// /* Store the original strength or armor value to find out the final change that this modifier applies. */
	// const int OriginalValue = NewModifier.EffectedStat ? PieceToModify->GetCurrentStrength() : PieceToModify->GetCurrentArmor();
	// int NewValue = OriginalValue;
	//
	// /* Save the new strength value to determine the modifier pop-up value. */
	// if (NewModifier.StrengthChange != 0)
	// 	NewValue = FMath::Clamp(PieceToModify->GetCurrentStrength() + NewModifier.StrengthChange, 0, 20);
	//
	// /* Save the new armor value to determine the modifier pop-up value. */
	// if (NewModifier.ArmorChange != 0)
	// 	NewValue = FMath::Clamp(PieceToModify->GetCurrentArmor() + NewModifier.ArmorChange, 0, 20);

	/* Set whether the new modifier will trigger pop-ups when added. */
	NewModifier.bArmPopUpPlayed = !bActivatePopUps;
	NewModifier.bStrPopUpPlayed = !bActivatePopUps;

	/* Get the target piece's current temporary modifiers. */
	TArray<FModifier>& TemporaryModifiers = PieceToModify->GetTemporaryModifiers();
	
	/* Check if this modifier instance or a modifier of this type is already applied. */
	int RepeatedInstanceIndex = -1;
	int RepeatedTypeIndex = -1;
	for (int i = 0; i < TemporaryModifiers.Num(); i++)
	{
		if (TemporaryModifiers[i] == NewModifier)
		{
			RepeatedInstanceIndex = i;
		}
		else if (TemporaryModifiers[i].equalTo(NewModifier))
		{
			RepeatedTypeIndex = i;
		}
	}

	/* If this exact modifier instance is already applied, don't apply it again. */
	if (RepeatedInstanceIndex != -1)
	{
		return;
	}
	/* If this type of modifier is already applied and can stack, reset its duration and stack the values together. */
	else if (RepeatedTypeIndex != -1 && NewModifier.bCanStack)
	{
		TemporaryModifiers[RepeatedTypeIndex].RemainingDuration = NewModifier.RemainingDuration;
		TemporaryModifiers[RepeatedTypeIndex].StrengthChange += NewModifier.StrengthChange;
		TemporaryModifiers[RepeatedTypeIndex].ArmorChange += NewModifier.ArmorChange;
	}
	/* If this type of modifier is already applied but can't stack, make a new instance of the modifier. */
	else if (RepeatedTypeIndex != -1 && !NewModifier.bCanStack)
	{
		const TArray<FModifier> OldTemporaryModifiers = TemporaryModifiers;
		TemporaryModifiers.Add(NewModifier);
		/* Manually call the OnRep for the server. */
		PieceToModify->OnRep_TemporaryModifiers(OldTemporaryModifiers);
	}
	/* If this type of modifier isn't already applied, apply it. */
	else
	{
		const TArray<FModifier> OldTemporaryModifiers = TemporaryModifiers;
		TemporaryModifiers.Add(NewModifier);
		/* Manually call the OnRep for the server. */
		PieceToModify->OnRep_TemporaryModifiers(OldTemporaryModifiers);
	}
}

void UPieceNetworkingComponent::Server_RemoveModifier_Implementation(AParentPiece* TargetPiece, FModifier ModifierToRemove,
	bool bActivatePopUps, bool bFlashHighlight)
{
	// /* Store the original strength or armor value to find out the final change that this modifier applies. */
	// const int OriginalValue = ModifierToRemove.EffectedStat ? TargetPiece->GetCurrentStrength() : TargetPiece->GetCurrentArmor();
	// int NewValue = OriginalValue;
	//
	// /* Save the new strength value to determine the modifier pop-up value. */
	// if (ModifierToRemove.StrengthChange != 0)
	// 	NewValue = FMath::Clamp(TargetPiece->GetCurrentStrength() - ModifierToRemove.StrengthChange, 0, 20);
	//
	// /* Save the new armor value to determine the modifier pop-up value. */
	// if (ModifierToRemove.ArmorChange != 0)
	// 	NewValue = FMath::Clamp(TargetPiece->GetCurrentArmor() - ModifierToRemove.ArmorChange, 0, 20);


	/* Some pieces' abilities that have lasting effects (i.e. modifiers) need to execute code when that effect ends. */
	const TArray<AActor*> Targets = { TargetPiece };
	if (AParentPiece* Piece = Cast<AParentPiece>(ModifierToRemove.SourceActor))
		Piece->OnAbilityEffectEnded(Targets);


	/* Remove the given modifier from the target piece, if it exists. */
	TArray<FModifier> &TemporaryModifiers = TargetPiece->GetTemporaryModifiers();
	const TArray<FModifier> OldTemporaryModifiers = TemporaryModifiers;
	TargetPiece->GetTemporaryModifiers().Remove(ModifierToRemove);
	/* Manually call the OnRep for the server. */
	TargetPiece->OnRep_TemporaryModifiers(OldTemporaryModifiers);

	// Flash pop-ups
}

void UPieceNetworkingComponent::Server_AdjustModifierDuration_Implementation(AParentPiece* TargetPiece,
	FModifier ModifierToAdjust, int AmountToChange, bool bActivatePopUps, bool bFlashHighlight)
{
	TArray<FModifier> &TemporaryModifiers = TargetPiece->GetTemporaryModifiers();

	/* Find the modifier to adjust in the piece's list of active modifiers. */
	for (int i = 0; i < TemporaryModifiers.Num(); i++)
	{
		if (ModifierToAdjust == TemporaryModifiers[i])
		{
			/* If the modifier to adjust has a definite duration. */
			if (!TemporaryModifiers[i].bIndefiniteDuration)
			{
				/* Change the given modifier's duration by the given amount. */
				TemporaryModifiers[i].RemainingDuration += AmountToChange;

				/* If the modifier's new duration is less than or equal to 0, remove it. */
				if (TemporaryModifiers[i].RemainingDuration < 1)
					Server_RemoveModifier(TargetPiece, ModifierToAdjust, bActivatePopUps, bFlashHighlight);

				break;
			}
		}
	}
}

void UPieceNetworkingComponent::BeginPlay()
{
	Super::BeginPlay();
}
