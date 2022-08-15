// Copyright Samuel Reitich 2022.

#pragma once

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
	E_SelectingTarget_PassiveAbility	UMETA(DisplayName="Selecting Target for Passive Ability")
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
	E_DefenderDeadlock			UMETA(DisplayName="Defender-Side Deadlock")
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
	class AParentPiece* Attacker;

	/* The target of the attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AParentPiece* Defender;

	/* If the attacker needs to move to the defender before attacking. Usually only false if the attack is ranged. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoveTo;

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