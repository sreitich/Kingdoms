// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_MatchmakingStartPopUp.h"

void UMM_MatchmakingStartPopUp::NativeConstruct()
{
	Super::NativeConstruct();

	/* Play the activation animation. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);
}
