// Copyright Samuel Reitich 2022.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_Parent.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UAnimInstance_Parent : public UAnimInstance
{
	GENERATED_BODY()

/* Public animations. */
public:
	
	/* The blend space to use in this animation's idle and walk animations. Each piece has a unique blend space for
	 * their unique idle and walk animations that they set this to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UBlendSpaceBase* IdleWalkBS;

	/* The animation played when this piece attacks, set by each piece. */
	UPROPERTY(BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* AttackAnimation;


/* Public animation triggers. */
public:

	/* Setting this to true triggers the attack animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Triggers")
	bool bAttack = false;
	
	
};
