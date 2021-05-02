// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerBlueprintLibrary.h"
#include "UI/GravRacerHUD.h"
//#include "GravRacerGameMode.h"

UGravRacerBlueprintLibrary::UGravRacerBlueprintLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

/*
AGravRacerGameMode* GetGameFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(MyWorld);

	AGravRacerGameMode* const MyGame = MyWorld->GetAuthGameMode<AGravRacerGameMode>();
	return MyGame;
}

void UGravRacerBlueprintLibrary::FinishRace(UObject* WorldContextObject)
{
	AGravRacerGameMode* const MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		MyGame->FinishRace();
	}
}

void UGravRacerBlueprintLibrary::SetInfoText(UObject* WorldContextObject, FString InfoText)
{
	AGravRacerGameMode* const MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		MyGame->SetGameInfoText(FText::FromString(InfoText));
	}
}

 void UGravRacerBlueprintLibrary::HideInfoText(UObject* WorldContextObject)
 {
 	AGravRacerGameMode* const MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		MyGame->SetGameInfoText(FText::GetEmpty());
	}
 }

 void UGravRacerBlueprintLibrary::ShowGameMenu(UObject* WorldContextObject)
 {
	 UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	 APlayerController* LocalPC = MyWorld ? GEngine->GetFirstLocalPlayerController(MyWorld) : nullptr;
	 AGravRacerHUD* MyHUD = LocalPC ? Cast<AGravRacerHUD>(LocalPC->GetHUD()) : nullptr;
	 if (MyHUD)
	 {
		 if (!MyHUD->IsGameMenuUp())
		 {
			 MyHUD->ToggleGameMenu();
		 }
	 }
 }

 void UGravRacerBlueprintLibrary::ShowHUD(UObject* WorldContextObject, bool bEnable)
 {
	 UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	 APlayerController* LocalPC = MyWorld ? GEngine->GetFirstLocalPlayerController(MyWorld) : nullptr;
	 AGravRacerHUD* MyHUD = LocalPC ? Cast<AGravRacerHUD>(LocalPC->GetHUD()) : nullptr;
	 if (MyHUD)
	 {
		 MyHUD->EnableHUD(bEnable);
	 }
 }*/


