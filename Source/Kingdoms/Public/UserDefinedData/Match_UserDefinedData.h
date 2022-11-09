// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "CoreMinimal.h"

#include "Match_UserDefinedData.generated.h"

/* What state this player is currently in. */
UENUM(BlueprintType)
enum EPlayerStatus
{
	E_Connecting						UMETA(DisplayName="Connecting"),
	E_PlacingPieces						UMETA(DisplayName="Placing Pieces"),
	E_WaitingForTurn					UMETA(DisplayName="Waiting For Turn"),
	E_SelectingPiece					UMETA(DisplayName="Selecting Piece"),
	E_SelectingAction					UMETA(DisplayName="Selecting Action"),
	E_SelectingTarget_Move 				UMETA(DisplayName="Selecting Enemy Target for Movement or Attack"),
	E_SelectingTarget_ActiveAbility		UMETA(DisplayName="Selecting Target for Active Ability"),
};

/* What state the match is currently in. */
UENUM(BlueprintType)
enum EMatchStatus
{
	E_WaitingForPlayers		UMETA(DisplayName="Waiting For Players"),
	E_SettingUpPieces 		UMETA(DisplayName="Setting Up Pieces"),
	E_Player1Turn			UMETA(DisplayName="Player 1 Turn"),
	E_Player2Turn			UMETA(DisplayName="Player 2 Turn"),
	E_EndingGame			UMETA(DisplayName="Ending Game")
};

/* Animations that can be played in an attack graphic. */
UENUM(BlueprintType)
enum EAttackGraphicAnimation
{
	E_NeutralFightInitiation	UMETA(DisplayName="Neutral Fight Initiation"),
	E_AttackerFightInitiation	UMETA(DisplayName="Attacker-Only Fight Initiation"),
	E_AttackerDeath 			UMETA(DisplayName="Attacker Death"),
	E_DefenderDeath				UMETA(DisplayName="Defender Death"),
	E_AttackerDeadlock 			UMETA(DisplayName="Attacker-Side Deadlock"),
	E_DefenderDeadlock			UMETA(DisplayName="Defender-Side Deadlock"),
	E_BothSidesDeadlock			UMETA(DisplayName="Both Sides Deadlock")
};

/* Textures that can be displayed in a billboard attack pop-up. */
UENUM(BlueprintType)
enum EAttackBillboardPopUpTexture
{
	E_AttackerIndicator		UMETA(DisplayName="Attacker Indicator"),
	E_DefenderIndicator		UMETA(DisplayName="Defender Indicator"),
	E_DeathIndicator 		UMETA(DisplayName="Death Indicator"),
	E_VictoryIndicator		UMETA(DisplayName="Victory Indicator"),
	E_DeadlockIndicator 	UMETA(DisplayName="Deadlock Indicator")
};

UENUM(BlueprintType)
enum EAttackResult
{
	E_AttackerSurvivesDefenderDies		UMETA(DisplayName="Attacker kills defender and survives"),
	E_DefenderSurvivesAttackerDies		UMETA(DisplayName="Defender kills attacker and survives"),
	E_AttackerDiesDefenderDies			UMETA(DisplayName="Attacker kills defender and defender kills attacker"),
	E_AttackerSurvivesDefenderSurvives	UMETA(DisplayName="Attacker survives and defender survives"),
	E_DefenderSurvives					UMETA(DisplayName="Defender survives without fighting back")
};

/* Coordinates for board tiles and pieces. */
USTRUCT(BlueprintType)
struct FTileCoordinates
{
	GENERATED_BODY()

	/* X-coordinate (rows). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X = 0;

	/* Y-coordinate (columns). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y = 0;
};

/* Information needed to execute an attack. */
USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()
	
	/* The piece initiating the attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AParentPiece* Attacker = nullptr;

	/* The target of the attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AParentPiece* Defender = nullptr;

	/* If the attacker needs to move to the defender before attacking. Usually only false if the attack is ranged. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoveTo;

	/* If the attacker has moved to the defender because bMoveTo was true and it was far enough away. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoved;
	
	/* If the defender fights back against the attacker. False indicates that this is a one-sided attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDefenderFights;

	/* If the attacker should take the place of the defender (move to their tile) if the defender dies and the attacker
	 * survives. This should always be true for an attack triggered by moving a piece onto an occupied tile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTakePlace;
};

/* Information used to interpolate the camera from one transform to another. */
USTRUCT(BlueprintType)
struct FCameraInterpolationInfo
{
	GENERATED_BODY()

	/* Location to start at or return to. */
	UPROPERTY(BlueprintReadWrite)
	FVector StartLocation;

	/* Location to move to. */
	UPROPERTY(BlueprintReadWrite)
	FVector EndLocation;

	/* Rotation to start at or return to. */
	UPROPERTY(BlueprintReadWrite)
	FRotator StartRotation;

	/* Rotation to move to. */
	UPROPERTY(BlueprintReadWrite)
	FRotator EndRotation;

	/* Length to start the camera's spring arm at or to return it to. */
	UPROPERTY(BlueprintReadWrite)
	float StartArmLength;

	/* Length to interpolate the camera's spring arm to. */
	UPROPERTY(BlueprintReadWrite)
	float EndArmLength;

	/* Whether to interpolate to or from the ending location, rotation, and arm length. */
	UPROPERTY(BlueprintReadWrite)
	bool bReverse;
};

USTRUCT(BlueprintType)
struct FModifier
{
	GENERATED_BODY()

	/* Assigned to ModifiedStat. */
	static const int Armor = 0;
	static const int Strength = 1;

	/* The actor (usually a piece) that caused the modifier. */
	UPROPERTY(BlueprintReadWrite)
	AActor* SourceActor;

	/* The alignment of the actor responsible for the modifier, relative to the player whose piece is being modified. */
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EAlignment> SourceAlignmentToTarget;

	/* The name of the actor that caused the modifier. */
	UPROPERTY(BlueprintReadWrite)
	FString SourceName;

	/* The name of the ability or effect that caused the modifier. */
	UPROPERTY(BlueprintReadWrite)
	FString SourceAbilityName;

	/* Which stat is being modified. 0 is armor, 1 is strength. */
	UPROPERTY(BlueprintReadWrite)
	int EffectedStat;

	/* The net effect of the modifier on the affected piece's strength. */
	UPROPERTY(BlueprintReadWrite)
	int StrengthChange;

	/* The net effect of the modifier on the affected piece's armor. */
	UPROPERTY(BlueprintReadWrite)
	int ArmorChange;

	/* Determines whether this modifier's duration is determined by a fixed duration or by a different condition. */
	UPROPERTY(BlueprintReadWrite)
	bool bIndefiniteDuration;

	/* The remaining turn duration of the modifier. */
	UPROPERTY(BlueprintReadWrite)
	int RemainingDuration;

	/* Two modifiers are equal if they have the same source actor, source ability, and alignment to the target. */
	bool operator==(const FModifier &Other) const
	{
		return
			SourceActor == Other.SourceActor &&
			SourceAbilityName == Other.SourceAbilityName &&
			SourceAlignmentToTarget == Other.SourceAlignmentToTarget;
	}
};