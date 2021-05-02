// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/GravRacerHUD.h"
#include "UI/Style/GravRacerStyle.h"
#include "Widgets/SGravRacerHUDWidget.h"
#include "Menu/SGravRacerMenuWidget.h"
#include "Menu/GravRacerMenuItem.h"
#include "Widgets/SGravRacerControlsSetup.h"
#include "UI/Style/GravRacerMenuSoundsWidgetStyle.h"
#include "GravRacerUserSettings.h"
#include "GASShooterGameModeBase.h"

#define LOCTEXT_NAMESPACE "GravRacer.hUD.Menu"

AGravRacerHUD::AGravRacerHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> SpeedMeterObj(TEXT("/Game/UI/HUD/Materials/M_VH_HUD_SpeedMeter_UI"));

	static ConstructorHelpers::FObjectFinder<UFont> HUDFontOb(TEXT("/Game/UI/HUD/UI_Vehicle_Font"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> TimerBgObj(TEXT("/Game/UI/HUD/Background/T_VH_Hud_Timer_Background"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> PlaceBgObj(TEXT("/Game/UI/HUD/Background/T_VH_Hud_Place_Background"));


	static ConstructorHelpers::FObjectFinder<UTexture2D> UpButtonTextureOb(TEXT("/Game/UI/HUD/UpButton"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DownButtonTextureOb(TEXT("/Game/UI/HUD/DownButton"));

	UpButtonTexture = UpButtonTextureOb.Object;
	DownButtonTexture = DownButtonTextureOb.Object;

	HUDFont = HUDFontOb.Object;

	TimerBackground = TimerBgObj.Object;
	PlaceBackground = PlaceBgObj.Object;
	
	SpeedMeterMaterialConst = SpeedMeterObj.Object;


	LowHighList.Add(LOCTEXT("LowQuality","LOW QUALITY"));
	LowHighList.Add(LOCTEXT("HighQuality","HIGH QUALITY"));

	UIScale = 1.0f;

	CurrentLetter = 0;
	bEnterNamePromptActive = false;
	bDrawHUD = true;
}


void AGravRacerHUD::BeginPlay()
{
	SpeedMeterMaterial = UMaterialInstanceDynamic::Create(SpeedMeterMaterialConst, nullptr);
}

void AGravRacerHUD::DrawHUD()
{
	Super::DrawHUD();
	BuildMenuWidgets();
	if (GetNetMode() != NM_Standalone)
	{
		FString NetModeDesc = (GetNetMode() == NM_Client) ? TEXT("Client") : TEXT("Server");
		DrawDebugInfoString(NetModeDesc, 256.0f,32.0f, true, true, FColor::White);
	}
}

void AGravRacerHUD::DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor)
{
#if !UE_BUILD_SHIPPING
	float SizeX, SizeY;
	UFont* Font = GEngine->GetSmallFont();
	Canvas->StrLen(Font, Text, SizeX, SizeY);

	const float X = PosX;
	const float Y = PosY;
	FCanvasTileItem TileItem(FVector2D(X, Y), FVector2D(SizeX, SizeY), FLinearColor::Gray);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(Text), Font, TextColor);
	TextItem.EnableShadow(FLinearColor::Black);
	FFontRenderInfo FontInfo;
	FontInfo.bEnableShadow = true;
	TextItem.FontRenderInfo = FontInfo;
	Canvas->DrawItem(TextItem);
#endif
}

void AGravRacerHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);

	if (BoxName.GetPlainNameString() == "Letter")
	{
		CurrentLetter = BoxName.GetNumber();
	}
	if (BoxName == "Up")
	{
		if (HighScoreName[CurrentLetter] < 'Z')
		{
			HighScoreName[CurrentLetter]++;
		}
	}
	if (BoxName == "Down")
	{
		if (HighScoreName[CurrentLetter] > 'A')
		{
			HighScoreName[CurrentLetter]--;
		}
	}
	if (BoxName == "OK")
	{
		bEnterNamePromptActive = false;
		if (PlayerOwner)
		{
			PlayerOwner->bShowMouseCursor = bEnterNamePromptActive;
		}

		FString EnteredName = FString();
		for (int32 i=0; i < HighScoreName.Num(); i++)
		{
			EnteredName.AppendChar(HighScoreName[i]);
		}
	}
}


void AGravRacerHUD::BuildMenuWidgets()
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}
	int32 CurrentQuality = 1;
	UGravRacerUserSettings* UserSettings = nullptr;

	UserSettings = CastChecked<UGravRacerUserSettings>(GEngine->GetGameUserSettings());
	if (UserSettings)
	{
		CurrentQuality = UserSettings->GetGraphicsQuality();
	}

	if (!GravRacerHUDWidget.IsValid())
	{
		SAssignNew(GravRacerHUDWidget, SGravRacerHUDWidget)
		.OwnerWorld(GetWorld());

		GEngine->GameViewport->AddViewportWidgetContent(
			SNew(SWeakWidget)
			.PossiblyNullContent(GravRacerHUDWidget.ToSharedRef())
			);

	}

	if (!MyHUDMenuWidget.IsValid())
	{
		SAssignNew(MyHUDMenuWidget, SGravRacerMenuWidget)
			.Cursor(EMouseCursor::Default)
			.MenuHUD(this)
			.HideBackground(true);

		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("RestartTrack", "RESTART TRACK"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD::ExecuteMenuAction, EGravRacerHUD::RestartTrack);
		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Options", "OPTIONS"));
		QualityMenuItem = SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LowHighList[CurrentQuality]);
		QualityMenuItem->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD::ExecuteMenuAction, EGravRacerHUD::ChangeQuality);
		SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("KeyboardControls", "KEYBOARD CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD::ExecuteMenuAction, EGravRacerHUD::RemapControls);
		SGravRacerMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("ControllerControls", "CONTROLLER CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD::ExecuteMenuAction, EGravRacerHUD::RemapController);
		SGravRacerMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Quit", "QUIT"))
			->OnConfirmMenuItem.BindUObject(this, &AGravRacerHUD::ExecuteMenuAction, EGravRacerHUD::Quit);

		MyHUDMenuWidget->MainMenu = MyHUDMenuWidget->CurrentMenu = MainMenu;
		MyHUDMenuWidget->OnMenuHidden.BindUObject(this,&AGravRacerHUD::DetachGameMenu);
		MyHUDMenuWidget->OnToggleMenu.BindUObject(this,&AGravRacerHUD::ToggleGameMenu);

		//this menu is not connected with the rest
		SGravRacerMenuWidget::AddCustomWidgetMenuItem(ControlsMenu,SAssignNew(ControlsSetupWidget,SGravRacerControlsSetup).OwnerWidget(MyHUDMenuWidget).OwnerHUD(this));

		bIsGameMenuUp = false;
	}
}

void AGravRacerHUD::DetachGameMenu()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(GameMenuContainer.ToSharedRef());
	}
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	bIsGameMenuUp = false;
	PlayerOwner->SetCinematicMode(bIsGameMenuUp,false,false,true,true);
}

bool AGravRacerHUD::IsGameMenuUp() const
{
	return bIsGameMenuUp;
}

void AGravRacerHUD::EnableHUD(bool bEnable)
{
	bDrawHUD = bEnable;
}

void AGravRacerHUD::ToggleGameMenu()
{
	if (!MyHUDMenuWidget.IsValid())
	{
		return;
	}

	if (bIsGameMenuUp && MyHUDMenuWidget->CurrentMenu != MainMenu)
	{
		MyHUDMenuWidget->MenuGoBack();
		return;
	}

	// Make the game pause (network games will not pause!)
	AGASShooterGameModeBase* const MyGame = GetWorld()->GetAuthGameMode<AGASShooterGameModeBase>();
	if( MyGame )
	{
		//TODO
		//MyGame->SetGamePaused( !bIsGameMenuUp );
	}

	if (!bIsGameMenuUp)
	{
		GEngine->GameViewport->AddViewportWidgetContent(
			SAssignNew(GameMenuContainer,SWeakWidget)
			.PossiblyNullContent(MyHUDMenuWidget.ToSharedRef())
			);
		MyHUDMenuWidget->BuildAndShowMenu();
		bIsGameMenuUp = true;
		PlayerOwner->SetCinematicMode(bIsGameMenuUp,false,false,true,true);
	} 
	else 
	{
		MyHUDMenuWidget->HideMenu();
		bIsGameMenuUp = false;
	}
}

void AGravRacerHUD::RestartTrack()
{
	GetOwningPlayerController()->RestartLevel();
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void AGravRacerHUD::ReturnToMenu()
{
// 	FString RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.").ToString();
	FText LocalReturnReason = FText::GetEmpty();

	if (GetNetMode() < NM_Client)
	{
		APlayerController* Controller = nullptr;
		FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
		Iterator.Reset();
		for(; Iterator; ++Iterator)
		{
			Controller = Iterator->Get();
			if (Controller && Controller->IsLocalPlayerController() && Controller->IsPrimaryPlayer())
			{
				Controller->ClientReturnToMainMenuWithTextReason(LocalReturnReason);
			}
		}
	}
	else
	{
		GetOwningPlayerController()->ClientReturnToMainMenuWithTextReason(LocalReturnReason);
	}
}

void AGravRacerHUD::Quit()
{
	GetOwningPlayerController()->ConsoleCommand("quit");
}

void AGravRacerHUD::ExecuteMenuAction(EGravRacerHUD::Type Action)
{
	UGravRacerUserSettings* UserSettings = CastChecked<UGravRacerUserSettings>(GEngine->GetGameUserSettings());
	int32 NewQuality = (UserSettings->GetGraphicsQuality() == 1 ? 0 : 1);

	const FGravRacerMenuSoundsStyle& MenuSounds = FGravRacerStyle::Get().GetWidgetStyle<FGravRacerMenuSoundsStyle>("DefaultGravRacerMenuSoundsStyle");

	switch (Action)
	{
	case EGravRacerHUD::RestartTrack:
		MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.StartGameSound,this,&AGravRacerHUD::RestartTrack);
		MyHUDMenuWidget->MenuGoBack();
		break;
	case EGravRacerHUD::RemapControls:
		ControlsSetupWidget->UpdateActionBindings(FName("Keyboard"));
		MyHUDMenuWidget->NextMenu = ControlsMenu;
		MyHUDMenuWidget->EnterSubMenu();
		break;
	case EGravRacerHUD::RemapController:
		ControlsSetupWidget->UpdateActionBindings(FName("Controller"));
		MyHUDMenuWidget->NextMenu = ControlsMenu;
		MyHUDMenuWidget->EnterSubMenu();
		break;
	case EGravRacerHUD::ChangeQuality:
		if (UserSettings)
		{
			FSlateApplication::Get().PlaySound(MenuSounds.AcceptChangesSound);
			UserSettings->SetGraphicsQuality(NewQuality);
			QualityMenuItem->Text = LowHighList[NewQuality];
			UserSettings->ApplySettings(false);
		}
		break;
	case EGravRacerHUD::Quit:
		MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.ExitGameSound,this,&AGravRacerHUD::Quit);
		break;

	default:
		MyHUDMenuWidget->MenuGoBack();
		break;
	}
}

#undef LOCTEXT_NAMESPACE
