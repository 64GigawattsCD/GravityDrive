// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "GravRacerTypes.h"
#include "GravRacerHUD.generated.h"

class AVehicleTrackPoint;
class SWeakWidget;
class SGravRacerMenuWidget;
class SGravRacerHUDWidget;
class SGravRacerControlsSetup;

namespace EGravRacerHUD
{
	enum Type
	{
		RestartTrack,
		RemapControls,
		RemapController,
		ChangeQuality,
		ReturnToMainMenu,
		Quit,
	};
};

namespace ETextAlignH
{
	enum Type
	{
		Left,
		Center,
		Right
	};
};

namespace ETextAlignV
{
	enum Type
	{
		Top,
		Center,
		Bottom
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighscoreNameAccepted, const FString&, NewHighscoreName);

UCLASS()
class AGravRacerHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	MenuPtr MainMenu;

	MenuPtr ControlsMenu;

	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

public:
	void ToggleGameMenu();

	bool IsGameMenuUp() const;

	void EnableHUD(bool bEnable);

protected:

	bool bDrawHUD;

	bool bIsGameMenuUp;

	TWeakObjectPtr<AVehicleTrackPoint> LastTrackPoint;

	TSharedPtr<SWeakWidget> GameMenuContainer;

	TSharedPtr<SGravRacerMenuWidget> MyHUDMenuWidget;

	TSharedPtr<SGravRacerHUDWidget> GravRacerHUDWidget;

	TSharedPtr<SGravRacerControlsSetup> ControlsSetupWidget;

	FGravRacerMenuItem* QualityMenuItem;

	TArray<FText> LowHighList;

	virtual void NotifyHitBoxClick(FName BoxName) override;

	void ExecuteMenuAction(EGravRacerHUD::Type Action);

	void BuildMenuWidgets();

	void DetachGameMenu();
	
	void RestartTrack();

	void ReturnToMenu();

	void Quit();

	void DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor);

	UPROPERTY()
	UMaterialInstanceConstant* SpeedMeterMaterialConst;

	UPROPERTY()
	UMaterialInstanceDynamic* SpeedMeterMaterial;

	UPROPERTY()
	UFont* HUDFont;

	UPROPERTY()
	UTexture2D* TimerBackground;

	UPROPERTY()
	UTexture2D* PlaceBackground;

	TArray<char> HighScoreName;

	uint8 CurrentLetter;

	uint8 bEnterNamePromptActive : 1;

	UTexture2D* UpButtonTexture;

	UTexture2D* DownButtonTexture;

	float UIScale;
};

