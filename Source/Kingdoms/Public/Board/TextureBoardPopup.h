// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TextureBoardPopup.generated.h"

class UBillboardComponent;

UCLASS()
class KINGDOMS_API ATextureBoardPopup : public AActor
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this actor's properties. */
	ATextureBoardPopup();
	
	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Updates the displayed texture, plays an animation to reveal it, and destroys this actor after the given
	 * duration. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ActivatePopup(UTexture2D* Texture, float Duration);


/* Public components. */
public:

	/* Root of the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneRoot;

	/* The billboard that displays the popup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UBillboardComponent* DisplayedTexture;


/* Public assets. */
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UTexture2D* AttackerIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UTexture2D* DefenderIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UTexture2D* DeathIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UTexture2D* VictoryIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UTexture2D* DeadlockIndicator;
	

/* Protected functions. */
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
