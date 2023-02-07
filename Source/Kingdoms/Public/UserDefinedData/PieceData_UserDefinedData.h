// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"

#include "PieceData_UserDefinedData.generated.h"

/* Piece classes. */
UENUM(BlueprintType)
enum EPieceClass
{
	E_Soldier		UMETA(DisplayName="Soldier"),
	E_Mage			UMETA(DisplayName="Mage"),
	E_Paladin		UMETA(DisplayName="Paladin"),
	E_Rogue			UMETA(DisplayName="Rogue"),
	E_Engineer		UMETA(DisplayName="Engineer"),
	E_Commander		UMETA(DisplayName="Commander")
};

/* Rarities of pieces. */
UENUM(BlueprintType)
enum EPieceRarity
{
	E_Basic		UMETA(DisplayName="Basic"),
	E_Superior	UMETA(DisplayName="Superior"),
	E_Heroic	UMETA(DisplayName="Heroic"),
	E_Champion	UMETA(DisplayName="Champion")
};

/* The alignment of a piece relative to the local player. */
UENUM(BlueprintType)
enum EAlignment
{
	E_Friendly	UMETA(DisplayName="Friendly"),
	E_Hostile	UMETA(DisplayName="Hostile"),
	E_Neutral	UMETA(DisplayName="Neutral"),
	E_DebugAlignment		UMETA(DisplayName="Debug Alignment")
};

/* The structure of the piece data table. */
USTRUCT(BlueprintType)
struct FPieceDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	/* This piece's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Info")
	FString PieceName = "";

	/* This piece's class-type (soldier, paladin, mage, rogue, engineer, commander). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Info")
	TEnumAsByte<EPieceClass> ClassType = E_Soldier;

	/* This piece's rarity (basic, superior, heroic, champion). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Info")
	TEnumAsByte<EPieceRarity> Rarity = E_Basic;

	/* The armor value that this piece starts with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Info")
	int BaseArmor = 0;

	/* The strength value that this piece starts with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Info")
	int BaseStrength = 0;

	/* This piece's displayed description. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Info")
	FString PieceDes = "";
	
	/* The reference to this piece's blueprint class, used when spawning the piece. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Code")
	TSubclassOf<class AParentPiece> ClassRef;

	/* The photo used on a piece's card when placing pieces before a match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Photos")
	UTexture2D* PieceCardPortrait = nullptr;

	/* The photo displayed when a piece is viewed in a match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Photos")
	UTexture2D* PieceInfoPhoto = nullptr;

	/* The photo displayed when previewing an attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Photos")
	UTexture2D* AttackPreviewPhoto = nullptr;

	/* This piece's active ability's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	FString ActiveName = "";

	/* This piece's active ability's displayed description. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	FString ActiveDes = "";

	/* Whether or not this piece's active ability targets tiles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	bool bActiveTargetsTiles = false;

	/* This piece's active ability's base cooldown. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	int ActiveCD = 0;

	/* How many times this piece's active ability can be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	int ActiveUses = 0;

	/* The displayed icon for this piece's active ability. This gets set in blueprints. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Photos")
	UTexture2D* ActiveIcon = nullptr;

	/* This piece's passive ability's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	FString PassiveName = "";

	/* This piece's passive ability's displayed description. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	FString PassiveDes = "";

	/* Whether or not this piece's passive ability targets tiles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	bool bPassiveTargetsTiles = false;

	/* This piece's passive ability's base cooldown. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	int PassiveCD = 0;

	/* How many times this piece's passive ability can be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	int PassiveUses = 0;

	/* The displayed icon for this piece's passive ability. This gets set in blueprints. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Photos")
	UTexture2D* PassiveIcon = nullptr;

};