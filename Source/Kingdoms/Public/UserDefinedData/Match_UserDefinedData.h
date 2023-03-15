// Copyright Change Studios, LLC 2023.

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
	E_Matchmaking			UMETA(DisplayName="Matchmaking"),
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
	E_DefenderSurvives					UMETA(DisplayName="Defender survives without fighting back"),
	E_DebugAttack						UMETA(DisplayName="Debug Attack")
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

/* Information used to update attack preview/confirmation widgets. */
USTRUCT(BlueprintType)
struct FAttackPreviewInfo
{
	GENERATED_BODY()

	/* The attacking piece (will be always be friendly to the local player). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AParentPiece* FriendlyPiece = nullptr;
	/* The defending piece (will always be hostile to the local player). This can be left as null to create a preview
	 * without a selected piece (i.e. the player is currently selecting one). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AParentPiece* EnemyPiece = nullptr;

	/* The attacker's overridden stats. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackerStrength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackerArmor = 0;

	/* The defender's overridden stats. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DefenderStrength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DefenderArmor = 0;

	/* Whether the previewed attack is one-sided. This affects the "attack outcome" text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOneSidedAttack = false;
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
	bool bMoveTo = false;

	/* If the attacker has moved to the defender because bMoveTo was true and it was far enough away. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoved = false;
	
	/* If the defender fights back against the attacker. False indicates that this is a one-sided attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDefenderFights = false;

	/* If the attacker should take the place of the defender (move to their tile) if the defender dies and the attacker
	 * survives. This should always be true for an attack triggered by moving a piece onto an occupied tile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTakePlace = false;
};

/* Information used to interpolate the camera from one transform to another. */
USTRUCT(BlueprintType)
struct FCameraInterpolationInfo
{
	GENERATED_BODY()

	/* Location to start at or return to. */
	UPROPERTY(BlueprintReadWrite)
	FVector StartLocation = FVector();

	/* Location to move to. */
	UPROPERTY(BlueprintReadWrite)
	FVector EndLocation = FVector();

	/* Rotation to start at or return to. */
	UPROPERTY(BlueprintReadWrite)
	FRotator StartRotation = FRotator();

	/* Rotation to move to. */
	UPROPERTY(BlueprintReadWrite)
	FRotator EndRotation = FRotator();

	/* Length to start the camera's spring arm at or to return it to. */
	UPROPERTY(BlueprintReadWrite)
	float StartArmLength = 0.0f;

	/* Length to interpolate the camera's spring arm to. */
	UPROPERTY(BlueprintReadWrite)
	float EndArmLength = 0.0f;

	/* Whether to interpolate to or from the ending location, rotation, and arm length. */
	UPROPERTY(BlueprintReadWrite)
	bool bReverse = false;
};

/* Information used to modify a piece's statistics as a result of an ability or effect. */
USTRUCT(BlueprintType)
struct FModifier
{
	GENERATED_BODY()

	/* DEPRECATED: Use StrengthChange and ArmorChange instead.
	 * Assigned to ModifiedStat. */
	static const int Armor = 0;
	static const int Strength = 1;

	/* The actor (usually a piece) that caused the modifier. */
	UPROPERTY(BlueprintReadWrite)
	AActor* SourceActor = nullptr;

	/* The alignment of the actor responsible for the modifier, relative to the player whose piece is being modified. */
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EAlignment> SourceAlignmentToTarget = E_Friendly;

	/* The name of the actor that caused the modifier. */
	UPROPERTY(BlueprintReadWrite)
	FString SourceName = FString();

	/* The name of the ability or effect that caused the modifier. */
	UPROPERTY(BlueprintReadWrite)
	FString SourceAbilityName = FString();

	/* Which stat is being modified. 0 is armor, 1 is strength. This is pending deprecated. It is required for the
	 * current statistic update system. Once the system is reworked, this will be removed. */
	UPROPERTY(BlueprintReadWrite)
	int EffectedStat = 0;

	/* The net effect of the modifier on the affected piece's strength. */
	UPROPERTY(BlueprintReadWrite)
	int StrengthChange = 0;

	/* The net effect of the modifier on the affected piece's armor. */
	UPROPERTY(BlueprintReadWrite)
	int ArmorChange = 0;

	/* Determines whether this modifier's duration is determined by a fixed duration or by a different condition. */
	UPROPERTY(BlueprintReadWrite)
	bool bIndefiniteDuration = false;

	/* The remaining turn duration of the modifier. */
	UPROPERTY(BlueprintReadWrite)
	int RemainingDuration = 0;

	/* Tracks if this modifier's strength modification pop-up has been played yet. Set this to true to avoid playing strength pop-ups. */
	UPROPERTY(BlueprintReadWrite)
	bool bStrPopUpPlayed = true;

	/* Tracks if this modifier's armor modification pop-up has been played yet. Set this to true to avoid playing armor pop-ups. */
	UPROPERTY(BlueprintReadWrite)
	bool bArmPopUpPlayed = true;

	/* Two modifiers are equal if they have the same source actor, source ability, and alignment to the target. */
	bool operator==(const FModifier &Other) const
	{
		return
			SourceActor == Other.SourceActor &&
			SourceAbilityName == Other.SourceAbilityName &&
			SourceAlignmentToTarget == Other.SourceAlignmentToTarget;
	}
};