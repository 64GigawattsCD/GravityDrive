// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRAVRACER_API UGRLoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "LoadingScreen")
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "LoadingScreen")
	void HideLoadingScreen();
};
