// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"

#include "SaveGameData_UserDefinedData.generated.h"

/* The structure of an army preset. */
USTRUCT(BlueprintType)
struct FArmyPresetStruct
{
    GENERATED_BODY()

    /* Name of this army preset. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ArmyName;

    /* List of piece IDs for each piece in the army. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Pieces;

    /* Default constructor. */
    FArmyPresetStruct()
    {
		ArmyName = "New Army";

	    if (Pieces.Num() > 0)
	    {
    		Pieces.Empty();
	    }
    }
};