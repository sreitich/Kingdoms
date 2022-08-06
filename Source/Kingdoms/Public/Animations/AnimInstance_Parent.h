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

/* Public animations. These are just variables that are plugged into the piece's locomotion system. Each piece sets the
 * animations for their animation instance at runtime using their static animation references. */
public:

	/* Composed of idle and walking animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations")
	UBlendSpaceBase* IdleWalkBS;

	UPROPERTY(BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* AttackAnimation;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* TakingDamageAnimation;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* DeathAnimation;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* CelebrationAnimation;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category="Animations")
	UAnimSequenceBase* DeadlockAnimation;


/* Public animation triggers. Setting these to "true" triggers the corresponding animation. */
public:

	/* Attack animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Triggers")
	bool bAttacking = false;

	/* Taking damage animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Triggers")
	bool bTakingDamage = false;

	/* Death animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Triggers")
	bool bDying = false;

	/* Celebration animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Triggers")
	bool bCelebrating = false;

	/* Deadlock animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Triggers")
	bool bDeadlocking = false;
	
	
};
