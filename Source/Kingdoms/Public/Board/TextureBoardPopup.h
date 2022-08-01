// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TextureBoardPopup.generated.h"

UCLASS()
class KINGDOMS_API ATextureBoardPopup : public AActor
{
	GENERATED_BODY()

/* Public functions. */
public:

	// Sets default values for this actor's properties
	ATextureBoardPopup();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ActivatePopup(UTexture2D* Texture);


/* Public components. */
public:

	/* Root of the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneRoot;

	/* The billboard that displays the popup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UBillboardComponent* DisplayedTexture;


/* Protected functions. */
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
