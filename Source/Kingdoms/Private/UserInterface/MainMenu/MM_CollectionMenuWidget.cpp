// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_CollectionMenuWidget.h"

#include "SaveGames/UnlockedPieces_SaveGame.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserInterface/MainMenu/MM_HUD.h"
#include "UserInterface/MainMenu/PieceCollectionListing.h"

#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

void UMM_CollectionMenuWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_CollectionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();


	/* Load the save game for unlocked pieces to display which pieces the player has unlocked. */
	UUnlockedPieces_SaveGame* UnlockedPiecesSave = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0))
	{
		UnlockedPiecesSave = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::LoadGameFromSlot(UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName(), 0));
	}
	else
	{
		/* TODO: Display a "problem retrieving pieces" error message. */
	}


	/* Bind the "back" button to deactivate this widget. */
	BackButton->OnClicked.AddDynamic(this, &UMM_CollectionMenuWidget::OnBackClicked);

	/* Play the activation animation to reveal this widget. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Bind the deactivation animation to destroy this widget when it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_CollectionMenuWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);

	/* Ensure the unlocked pieces save game was loaded and that the piece data table has been set. */
	if (IsValid(UnlockedPiecesSave) && PieceDataTable)
	{
		/* Iterate through every listing (i.e. every piece) and add a piece listing widget for it. */
		for (const FName PieceID : PieceDataTable->GetRowNames())
		{
			/* Get the piece's data. */
			static const FString ContextString(TEXT("Piece Data Struct"));
			const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

			/* Ensure the piece data was successfully retrieved. */
			if (PieceData)
			{
				/* Create a new piece listing widget, update its represented piece, and add it to the grid. */
				UPieceCollectionListing* NewListing = CreateWidget<UPieceCollectionListing>(GetOwningPlayer(), PieceListingClass, FName(*(PieceData->PieceName + " Piece Listing")));
				NewListing->UpdatePieceCollectionListing(*PieceData, UnlockedPiecesSave->GetUnlockedPieces().Contains(PieceID.ToString()));
				PieceListingGrid->AddChild(NewListing);

				/* Place the new widget into the correct grid position. */
				if (UUniformGridSlot* NewListingSlot = Cast<UUniformGridSlot>(NewListing->Slot))
				{
					const int CardIndex = PieceListingGrid->GetChildrenCount() - 1;
					const int Row = CardIndex / 5;
					const int Column = CardIndex % 5;

					NewListingSlot->SetRow(Row);
					NewListingSlot->SetColumn(Column);
				}
			}
		}
	}
}

void UMM_CollectionMenuWidget::NativeDestruct()
{
	/* Remove all of the piece listings when destroying this widget. */
	PieceListingGrid->ClearChildren();

	Super::NativeDestruct();
}

void UMM_CollectionMenuWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreateCollectionMenu(true);
}

void UMM_CollectionMenuWidget::OnBackClicked()
{
	/* To navigate back to the main menu, queue the menu, which will deactivate this menu and transition to the new
	 * one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_MainMenu);
}
