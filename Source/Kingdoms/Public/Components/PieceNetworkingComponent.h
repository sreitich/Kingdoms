// Copyright Samuel Reitich 2023.

#pragma once

#include "Components/TimelineComponent.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PieceNetworkingComponent.generated.h"

class AParentPiece;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KINGDOMS_API UPieceNetworkingComponent : public UActorComponent
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Sets default values for this component's properties. */
	UPieceNetworkingComponent();


/* Protected functions. */
protected:

	/* Called when the game starts. */
	virtual void BeginPlay() override;

};