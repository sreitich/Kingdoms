// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_ModifierBoardPopUpWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMatch_ModifierBoardPopUpWidget::UpdateModifierWidget(int NetModifierValue, bool bStrength)
{
	/* Format the stat change value to show its positivity. */
	FString ModifierValueStr = FString::FromInt(NetModifierValue);
	if (NetModifierValue > 0)
		ModifierValueStr.InsertAt(0, TEXT("+"));
	
	/* Update the displayed text. */
	ModifierValue->SetText(FText::FromString(ModifierValueStr));
	/* Change the text's color to indicate whether the stat change is positive or negative. */
	ModifierValue->SetColorAndOpacity(FSlateColor(NetModifierValue > 0 ? StatIncreaseColor : StatDecreaseColor));

	/* Update the image displaying the icon of the corresponding stat that was changed. */
	StatImage->SetBrushFromTexture(bStrength ? StrengthIcon : ArmorIcon, false);
}