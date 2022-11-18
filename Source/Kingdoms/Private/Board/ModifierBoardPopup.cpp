// Copyright Samuel Reitich 2023.


#include "Board/ModifierBoardPopup.h"

#include "Pieces/ParentPiece.h"

void AModifierBoardPopup::OnModifierPopUpFinished()
{
	/* Get a reference to the piece that is playing this pop-up. */
	if (AParentPiece* InstigatingPiece = Cast<AParentPiece>(GetInstigator()))
	{
		/* Iterate through the rest of the target piece's modifiers. */
		for (FModifier& Modifier : InstigatingPiece->GetTemporaryModifiers())
		{
			/* If the modifier modifies strength and hasn't played its strength pop-up, play it. */
			if (!Modifier.bStrPopUpPlayed && Modifier.StrengthChange != 0)
			{
				Modifier.bStrPopUpPlayed = true;

				InstigatingPiece->Multicast_CreateModifierPopUp(Modifier.StrengthChange, true, true);
				
				break;
			}

			/* If the modifier modifies armor and hasn't played its armor pop-up, play it. */
			if (!Modifier.bArmPopUpPlayed && Modifier.ArmorChange != 0)
			{
				Modifier.bArmPopUpPlayed = true;

				InstigatingPiece->Multicast_CreateModifierPopUp(Modifier.ArmorChange, false, true);

				break;
			}
		}

		/* If no more pop-ups have to be triggered, then this piece isn't playing them anymore. */
		InstigatingPiece->bIsModifierPopUpPlaying = false;
	}
}