// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchSetup_UnplacedPieceWidget.generated.h"

class UDataTable;
class UImage;
class UPieceDragWidget;
class UTextBlock;

struct FPieceDataStruct;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatchSetup_UnplacedPieceWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates this widget's information based on the ID of the piece it represents. */
	void UpdateUnplacedPieceWidget(FString NewPieceID);


/* Public variables. */
public:

	/* ID of the piece that this widget represents. */
	FString PieceID;


/* Protected functions. */
protected:

	/* Called when the game starts or when created. */
	virtual void NativeConstruct() override;

	/* Called when the mouse button was pressed on this widget. */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/* Called when this widget starts to get dragged. */
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;


/* Protected variables. */
protected:

	/* Pointer to the piece data table, used to retrieve information to about the piece this widget represents. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	UDataTable* PieceDataTable;

	/* Pointer to the represented piece's data in the piece data table. This gets defined when the widget is created. */
	FPieceDataStruct* PieceData;

	/* The class of the widget to be created when dragging this widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Drag")
	TSubclassOf<UPieceDragWidget> PieceDragWidgetClass;


/* Protected widgets. */
protected:

	/* The displayed name of the piece that this widget represents. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PieceName;

	/* The displayed image of the piece that this widget represents. Called a "CardPortrait" in files. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PieceImage;

};
