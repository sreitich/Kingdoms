// Copyright Change Studios, LLC 2023.

#pragma once

#include "BlueprintFunctionLibraries/SteamFriendsFunctionLibrary.h"
#include "UserDefinedData/Menu_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MM_HUD.generated.h"

class UGetSteamFriendsCallProxyBase;
class UMM_ArmyMenuWidget;
class UMM_CollectionMenuWidget;
class UMM_FriendInvitationPopUp;
class UMM_LobbyMenuWidget;
class UMM_MainMenuWidget;
class UMM_MapSelectionWidget;
class UMM_MatchmakingStartPopUp;
class UMM_OptionsWidget;
class UMM_PlayMenuWidget;
class UMM_QueueTimer;
class UMM_StoreWidget;

/**
 * 
 */
UCLASS()
class KINGDOMS_API AMM_HUD : public AHUD
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_HUD();

	/* Called every frame to draw the HUD. */
	virtual void DrawHUD() override;

	/* Deactivates the current menu and queues the next menu. When the current menu is deactivated, it will call
	 * ChangeMenus(). */
	UFUNCTION(BlueprintCallable)
	void QueueMenuChange(EMenuScreen NewMenu);

	/* Setter for QueuedMenu. */
	FORCEINLINE void SetQueuedMenu(EMenuScreen NewMenu) { QueuedMenu = NewMenu; }

	/* Safely destroys the current menu and creates a new menu from the next queued menu. This is only called by menus
	 * after they finish deactivating. */
	UFUNCTION()
	void ChangeMenus();

	/* Creates or destroys the main menu widget. */
	UFUNCTION(Category="Main Menu Widgets")
	void CreateMainMenuWidget(bool bDestroy);

		/* Creates or destroys the play menu widget. */
		UFUNCTION(Category="Main Menu Widgets")
		void CreatePlayMenuWidget(bool bDestroy);

			/* Creates or destroys the lobby menu widget. */
			UFUNCTION(Category="Main Menu Widgets")
			void CreateLobbyMenuWidget(bool bDestroy);

				/* Creates or destroys the map selection menu widget. */
				UFUNCTION(Category="Main Menu Widgets")
				void CreateMapSelectionWidget(bool bDestroy);

		/* Creates or destroys the army menu widget. */
		UFUNCTION(Category="Main Menu Widgets")
		void CreateArmyMenuWidget(bool bDestroy);

			UFUNCTION(Category="Main Menu Widgets")
			void CreateCollectionMenu(bool bDestroy);

		/* Creates or destroys the store widget. */
		UFUNCTION(Category="Main Menu Widgets")
		void CreateStoreWidget(bool bDestroy);

		/* Creates or destroys the options widget. */
		UFUNCTION(Category="Main Menu Widgets")
		void CreateOptionsWidget(bool bDestroy);

	/* Creates a matchmaking start pop-up widget. */
	UFUNCTION(Category="Main Menu Widgets")
	void CreateMatchmakingStartPopUpWidget();

	/* Creates a queue timer widget. */
	UFUNCTION(BlueprintCallable, Category="Queue Timer Widget")
	void CreateQueueTimerWidget(bool bDestroy);

		/* Called when an opponent is found, preventing the player from "cancelling" matchmaking until they enter their
		 * game. Wrapper function for queue timer widget's FoundOpponent function. */
		UFUNCTION(BlueprintCallable, Category="Queue Timer Widget")
		void HUD_FoundOpponent();

	/* Creates or destroys an invitation pop-up widget. */
	UFUNCTION(BlueprintImplementableEvent, Category="Invitation Pop-Up Widget")
	void CreateInvitationPopUpWidget(bool bDestroy, FSteamFriend Friend);


/* Protected functions. */
protected:

	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


/* Protected variables. */
protected:

	/* Tracks which menu is currently being displayed. Only one menu can be displayed at a time. */
	UPROPERTY()
	UUserWidget* CurrentMenu;

	/* When transitioning between menus, this will be the next menu opened when the current one closes. */
	UPROPERTY()
	TEnumAsByte<EMenuScreen> QueuedMenu;


/* References to created widgets. */
protected:

	UPROPERTY()
	UMM_ArmyMenuWidget* ArmyMenuWidget;

	UPROPERTY()
	UMM_CollectionMenuWidget* CollectionMenuWidget;

	UPROPERTY()
	UMM_LobbyMenuWidget* LobbyMenuWidget;

	UPROPERTY()
	UMM_MainMenuWidget* MainMenuWidget;

	UPROPERTY()
	UMM_MapSelectionWidget* MapSelectionWidget;

	UPROPERTY()
	UMM_OptionsWidget* OptionsWidget;

	UPROPERTY()
	UMM_PlayMenuWidget* PlayMenuWidget;

	UPROPERTY()
	UMM_StoreWidget* StoreWidget;

	UPROPERTY()
	UMM_MatchmakingStartPopUp* MatchmakingStartPopUpWidget;

	UPROPERTY()
	UMM_QueueTimer* QueueTimerWidget;


/* References to the class to spawn for each widget. */
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> ArmyMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> CollectionMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> LobbyMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> MapSelectionWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types");
	TSubclassOf<UUserWidget> MatchmakingStartPopUpClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> OptionsWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> PlayMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types");
	TSubclassOf<UUserWidget> QueueTimerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> StoreWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Class Types")
	TSubclassOf<UUserWidget> InvitationPopUpClass;
	

};
