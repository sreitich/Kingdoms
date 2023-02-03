// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MM_MatchmakingStartPopUp.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMM_MatchmakingStartPopUp : public UUserWidget
{
	GENERATED_BODY()

/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

/* Protected animations. */
protected:

	/* Played when this widget is spawned. */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* OnActivatedAnim;

};
