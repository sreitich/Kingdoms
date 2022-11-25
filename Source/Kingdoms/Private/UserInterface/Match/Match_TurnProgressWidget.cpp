// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_TurnProgressWidget.h"

#include "Components/Image.h"

void UMatch_TurnProgressWidget::UpdateActionIndicator(bool bUsed, bool bMoveAction) const
{
	/* Update the requested indicator with whether or not its action was used. */
	if (bMoveAction)
		MoveActionIndicator->SetBrushFromTexture(bUsed ? MoveUsedImage : MoveUnusedImage, false);
	else
		AbilityActionIndicator->SetBrushFromTexture(bUsed ? AbilityUsedImage : AbilityUnusedImage, false);
}
