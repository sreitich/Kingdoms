// Copyright Samuel Reitich 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

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

	/* Apply a new modifier ot this piece, activating a pop-up and flashing a piece highlight if requested. If the
	 * modifier already exists, reset the duration and stack the modifiers together. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Modifiers")
	void Server_AddModifier(AParentPiece* PieceToModify, FModifier NewModifier, bool bActivatePopUp, bool bFlashHighlight);

	// /* Remove an existing modifier from this piece, activating a pop-up and flashing a highlight if requested. */
	// UFUNCTION(Server, Reliable, BlueprintCallable, Category="Modifiers")
	// void Server_RemoveModifier(AParentPiece* TargetPiece, FModifier ModifierToRemove, bool bActivatePopUp, bool bFlashHighlight);


/* Protected functions. */
protected:

	/* Called when the game starts. */
	virtual void BeginPlay() override;

};