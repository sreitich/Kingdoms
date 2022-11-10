// Copyright Samuel Reitich 2023.


#include "Components/PieceNetworkingComponent.h"

UPieceNetworkingComponent::UPieceNetworkingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UPieceNetworkingComponent::BeginPlay()
{
	Super::BeginPlay();
	
}