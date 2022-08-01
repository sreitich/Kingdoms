// Copyright Samuel Reitich 2022.


#include "Board/TextureBoardPopup.h"

#include "Components/BillboardComponent.h"

// Sets default values
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

// Called every frame
void ATextureBoardPopup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void ATextureBoardPopup::BeginPlay()
{
	Super::BeginPlay();
	
}
