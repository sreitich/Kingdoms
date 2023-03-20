// Copyright Change Studios, LLC 2023.


#include "UserInterface/MatchSetup/MatchSetup_UnplacedPieceWidget.h"

#include "UserDefinedData/PieceData_UserDefinedData.h"
#include "UserInterface/MatchSetup/Dragging/PieceDragWidget.h"
#include "UserInterface/MatchSetup/Dragging/PieceDragObject.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define OUT

void UMatchSetup_UnplacedPieceWidget::UpdateUnplacedPieceWidget(FString NewPieceID)
{
	PieceID = NewPieceID;

	/* If the piece data table exists... */
	if (IsValid(PieceDataTable))
	{
		/* Get and store the data regarding the piece this widget represents. */
		static const FString ContextString(TEXT("Piece Data Struct"));
		PieceData = PieceDataTable->FindRow<FPieceDataStruct>(FName(PieceID), ContextString, true);
	}

	/* If this widget's piece's data was found in the data table... */
	if (PieceData)
	{
		/* Update the displayed name of the piece. */
		PieceName->SetText(FText::FromString(PieceData->PieceName));

		/* Update the displayed image of the piece. */
		PieceImage->SetBrushFromTexture(PieceData->PieceCardPortrait, true);
	}
}

FReply UMatchSetup_UnplacedPieceWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	/* Store the result of the drag detection as a reply. */
	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	/* Return the event reply as an FReply. */
	return Reply.NativeReply;
}

void UMatchSetup_UnplacedPieceWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	/* If the piece drag widget class has been set. */
	if (PieceDragWidgetClass)
	{
		/* Create a new widget to represent this widget while it's being dragged. */
        UPieceDragWidget* PieceDragWidget = CreateWidget<UPieceDragWidget>(GetOwningPlayer(), PieceDragWidgetClass, FName("Piece Drag Widget"));

		/* Tell the piece drag widget which class it will need to spawn. */
		PieceDragWidget->ClassToSpawn = PieceData->ClassRef;
        /* Pass a reference of this widget to the drag widget. */
		PieceDragWidget->RepresentedWidget = this;
		/* Update the drag widget's displayed information using this widget's piece ID. */
		PieceDragWidget->UnplacedPieceWidget->UpdateUnplacedPieceWidget(PieceID);
		
		/* Initialize the widget to spawn its piece and to hide the widget with an animation. */
		PieceDragWidget->InitializeWidget(true);

		/* Create a new drag operation object */
        UPieceDragObject* PieceDragObject = NewObject<UPieceDragObject>();
		/* Assign the new widget to be dragged. */
        PieceDragObject->DefaultDragVisual = PieceDragWidget;
		/* Anchor the widget's location to the mouse position. */
        PieceDragObject->Pivot = EDragPivot::MouseDown;
		/* Store a reference to this widget so it can be moved when the visual is dropped. */
		// PieceDragObject->Payload = this;

		/* Hide the original widget so that it looks like the new widget is actually this widget being dragged. */
		SetVisibility(ESlateVisibility::Hidden);

		/* Return this drag operation object. */
		OutOperation = PieceDragObject;
	}
}
