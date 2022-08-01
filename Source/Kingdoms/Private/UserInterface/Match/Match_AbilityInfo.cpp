// Copyright Samuel Reitich 2022.


#include "UserInterface/Match/Match_AbilityInfo.h"

#include "Runtime/UMG/Public/UMG.h"

#define OUT

void UMatch_AbilityInfo::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    /* Get the position of the player's mouse. */
    float MouseX, MouseY;
    GetOwningPlayer()->GetMousePosition(OUT MouseX, OUT MouseY);

    /* Set the position of the widget to the player's mouse coordinates. */
    SetPositionInViewport(FVector2D(MouseX, MouseY), true);
}

void UMatch_AbilityInfo::UpdateAbilityInfo(FString NewName, FString NewDescription)
{
    DisplayedAbilityName->SetText(FText::FromString(NewName));
    DisplayedAbilityDescription->SetText(FText::FromString(NewDescription));
}