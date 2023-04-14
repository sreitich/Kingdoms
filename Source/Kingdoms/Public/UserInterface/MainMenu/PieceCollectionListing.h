// Copyright Change Studios, LLC 2023.

#pragma once

#include "UserDefinedData/Match_UserDefinedData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PieceCollectionListing.generated.h"

class UButton;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UPieceCollectionListing : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates the piece that is represented and displayed by this widget. */
	UFUNCTION()
	void UpdatePieceCollectionListing(FPieceDataStruct NewPieceData, bool bOwned);


/* Protected variables. */
protected:

	/* The name of the piece that this widget represents. */
	UPROPERTY(BlueprintReadOnly, Category="Piece Data")
	FString RepresentedPieceName;


/* Protected widgets. */
protected:

	/* The image displaying a preview of this piece. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PreviewImage;

	/* This piece's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Name;

	/* Creates an information pop-up with this piece's info. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* InfoPopupButton;

	/* A widget that covers the piece listing to grey it out, indicating it is locked. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* UnownedCover;

};
