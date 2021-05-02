// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGravRacerHUDWidget.h"
#include "UI/Style/GravRacerStyle.h"
#include "GASShooterGameModeBase.h"

#define LOCTEXT_NAMESPACE "GravRacer.hUD"

void SGravRacerHUDWidget::Construct(const FArguments& InArgs)
{
	FSlateFontInfo StartMessageFontStyle("UI_Vehicle_Font", 32);

	OwnerWorld = InArgs._OwnerWorld;

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+SOverlay::Slot() // game menu background overlay
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.Visibility(this, &SGravRacerHUDWidget::GetInfoTextVisibility)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				[
					SNew(STextBlock)
					.TextStyle(FGravRacerStyle::Get(), "GravRacer.MenuTextStyle")
					.ColorAndOpacity(this,&SGravRacerHUDWidget::GetInfoTextColor)
					.Text(this,&SGravRacerHUDWidget::GetInfoText)
					.Font(StartMessageFontStyle)
				]
			]
		]
	];
}

FText SGravRacerHUDWidget::GetInfoText() const
{
	AGASShooterGameModeBase* const MyGame = OwnerWorld->GetAuthGameMode<AGASShooterGameModeBase>();
	//return MyGame->GetGameInfoText();
	//TODO
	return FText();
}

EVisibility SGravRacerHUDWidget::GetInfoTextVisibility() const
{
	AGASShooterGameModeBase* const MyGame = OwnerWorld->GetAuthGameMode<AGASShooterGameModeBase>();
	//return (MyGame && !MyGame->GetGameInfoText().IsEmpty()) ? EVisibility::Visible : EVisibility::Collapsed;
	//TODO
	return EVisibility::Hidden;
}

FSlateColor SGravRacerHUDWidget::GetInfoTextColor() const
{
	const float AnimSpeedModifier = 0.8f;
	float Alpha = FMath::Abs(FMath::Sin(OwnerWorld->GetTimeSeconds())*AnimSpeedModifier);
	return FLinearColor(1,1,1,Alpha);
}

#undef LOCTEXT_NAMESPACE