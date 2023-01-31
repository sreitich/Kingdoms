// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MM_PlayerController.generated.h"

class UMM_QueueTimer;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_PlayerController : public APlayerController
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_PlayerController();

	/* Creates and saves a reference to a queue timer widget. */
	UFUNCTION(Category="User Interface")
	void CreateQueueTimerWidget();


/* Protected functions. */
protected:

	/* Called when spawned. */
	virtual void BeginPlay() override;


/* Protected widgets. */
protected:

	/* A reference to a currently displayed queue timer widget. */
	UPROPERTY()
	UMM_QueueTimer* QueueTimerWidget;


/* Protected widget class references. */
protected:

	/* The class to spawn when creating a queue timer widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="User Interface");
	TSubclassOf<UUserWidget> QueueTimerClass;
};
