// Copyright Change Studios, LLC 2023.


#include "Board/TextureBoardPopup.h"

#include "Components/BillboardComponent.h"

ATextureBoardPopup::ATextureBoardPopup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Set up this actor's components. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	DisplayedTexture = CreateDefaultSubobject<UBillboardComponent>(TEXT("Displayed Texture"));
	DisplayedTexture->SetupAttachment(SceneRoot);

}

void ATextureBoardPopup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATextureBoardPopup::BeginPlay()
{
	Super::BeginPlay();
	
}
