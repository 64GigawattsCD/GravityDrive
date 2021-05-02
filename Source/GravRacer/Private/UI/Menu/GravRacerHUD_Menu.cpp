// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Menu/GravRacerHUD_Menu.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SGravRacerMenuWidget.h"
#include "GravRacerMenuItem.h"
#include "UI/Widgets/SGravRacerControlsSetup.h"
#include "UI/Style/GravRacerStyle.h"
#include "GravRacerLoadingScreen/Public/GravRacerLoadingScreen.h"
#include "UI/Style/GravRacerMenuSoundsWidgetStyle.h"
#include "GravRacerUserSettings.h"

#define LOCTEXT_NAMESPACE "GravRacer.hUD.Menu"

AGravRacerHUD_Menu::AGravRacerHUD_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LowHighList.Add(LOCTEXT("LowQuality","LOW QUALITY"));
	LowHighList.Add(LOCTEXT("HighQuality","HIGH QUALITY"));
}

void AGravRacerHUD_Menu::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Now that we are here, build our menu widget
	RebuildWidgets();
}

void AGravRacerHUD_Menu::PlaySolo()
{
	GEngine->SetClientTravel(GetWorld(), TEXT("/Game/Maps/VehicleEntry"), TRAVEL_Absolute);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	ShowLoadingScreen();
}

void AGravRacerHUD_Menu::HostLocal()
{
	GEngine->SetClientTravel(GetWorld(), TEXT("/Game/Maps/VehicleEntry?listen"), TRAVEL_Absolute);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	ShowLoadingScreen();
}

void AGravRacerHUD_Menu::Quit()
{
	GetOwningPlayerController()->ConsoleCommand("quit");
}

void AGravRacerHUD_Menu::ShowLoadingScreen()
{
	IGravRacerLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<IGravRacerLoadingScreenModule>("GravRacerLoadingScreen");
	LoadingScreenModule->StartInGameLoadingScreen();
}

void AGravRacerHUD_Menu::ExecuteMenuAction(EGravRacerMenu::Type Action)
{	
	UGravRacerUserSettings* UserSettings = CastChecked<UGravRacerUserSettings>(GEngine->GetGameUserSettings());
	int32 NewQuality = UserSettings->GetGraphicsQuality() == 1 ? 0 : 1;

	const FGravRacerMenuSoundsStyle& MenuSounds = FGravRacerStyle::Get().GetWidgetStyle<FGravRacerMenuSoundsStyle>("DefaultGravRacerMenuSoundsStyle");

	switch (Action)
	{
		case EGravRacerMenu::PlaySolo:
			MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.StartGameSound,this,&AGravRacerHUD_Menu::PlaySolo);
			break;
		case EGravRacerMenu::HostLocal:
			MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.StartGameSound,this,&AGravRacerHUD_Menu::HostLocal);
			break;
		case EGravRacerMenu::JoinLocal:
			GEngine->SetClientTravel(GetWorld(), TEXT("127.0.0.1"), TRAVEL_Absolute);
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
			break;
		case EGravRacerMenu::RemapControls:
			//show our special menu
			ControlsSetupWidget->UpdateActionBindings(FName("Keyboard"));
			MenuWidget->NextMenu = ControlsMenu;
			MenuWidget->EnterSubMenu();
			break;
		case EGravRacerMenu::RemapController:
			ControlsSetupWidget->UpdateActionBindings(FName("Controller"));
			MenuWidget->NextMenu = ControlsMenu;
			MenuWidget->EnterSubMenu();
			break;
		case EGravRacerMenu::ChangeQuality:
			if (UserSettings)
			{
				FSlateApplication::Get().PlaySound(MenuSounds.AcceptChangesSound);
				UserSettings->SetGraphicsQuality(NewQuality);
				QualityMenuItem->Text = LowHighList[NewQuality];
				UserSettings->ApplySettings(false);
			}
			break;
		case EGravRacerMenu::Quit:
			MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.ExitGameSound,this,&AGravRacerHUD_Menu::Quit);
			break;

		default:
			MenuWidget->MenuGoBack();
			break;
	}
}

void AGravRacerHUD_Menu::RebuildWidgets(bool bHotReload)
{
	MenuWidget.Reset();

	int32 CurrentQuality = 1;
	UGravRacerUserSettings* UserSettings = CastChecked<UGravRacerUserSettings>(GEngine->GetGameUserSettings());
	CurrentQuality = UserSettings->GetGraphicsQuality();

	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* GVC = GEngine->GameViewport;
		SAssignNew(MenuWidget, SGravRacerMenuWidget)
			.Cursor(EMouseCursor::Default)
			.MenuHUD(this)
			.HideBackground(false);

		GVC->AddViewportWidgetContent(
			SNew(SWeakWidget)
			.PossiblyNullContent(MenuWidget.ToSharedRef()));

		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("PlaySolo", "PLAY SOLO"));
		SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("Mountains", "MOUNTAINS"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::PlaySolo);
		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("MultiPlayer", "MULTIPLAYER"));
			SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("HostLocal", "HOST"))
				->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::HostLocal);
			SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("JoinLocal", "JOIN LOCALHOST"))
				->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::JoinLocal);
		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Options", "OPTIONS"));
		QualityMenuItem = SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LowHighList[CurrentQuality]);
		QualityMenuItem->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::ChangeQuality);
		SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("KeyboardControls", "KEYBOARD CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::RemapControls);
		SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("ControllerControls", "CONTROLLER CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::RemapController);
		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Quit", "QUIT"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD_Menu::ExecuteMenuAction, EGravRacerMenu::Quit);

		MenuWidget->MainMenu = MenuWidget->CurrentMenu = MainMenu;
		
		//this menu is not connected with the rest
		SGravRacerMenuWidget::AddCustomWidgetMenuItem(ControlsMenu,SAssignNew(ControlsSetupWidget,SGravRacerControlsSetup).OwnerWidget(MenuWidget));

		MenuWidget->BuildAndShowMenu();
	}
}


#undef LOCTEXT_NAMESPACE
