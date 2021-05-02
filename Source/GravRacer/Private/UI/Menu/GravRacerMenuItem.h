// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SGravRacerMenuItem.h"

namespace EGravRacerMenuItemType
{
	enum Type
	{
		Standard,
		CustomWidget,
	};
};


class FGravRacerMenuItem
{
public:
	/** menu item text */
	FText Text;

	/** sub menu if present */
	TSharedPtr< TArray<FGravRacerMenuItem> > SubMenu;

	/** menu item type */
	EGravRacerMenuItemType::Type MenuItemType;

	/** shared pointer to actual slate widget representing the menu item */
	TSharedPtr<SGravRacerMenuItem> Widget;

	/** shared pointer to actual slate widget representing the custom menu item, ie whole options screen */
	TSharedPtr<SWidget> CustomWidget;

	/** delegate declaration */
	DECLARE_DELEGATE(FOnConfirmMenuItem);

	/** delegate, which is executed by SGravRacerMenuWidget if user confirms this menu item */
	FOnConfirmMenuItem OnConfirmMenuItem;

	/** constructor accepting menu item text */
	FGravRacerMenuItem(FText _Text)
	{
		Text = _Text;
		MenuItemType = EGravRacerMenuItemType::Standard;
		SubMenu = MakeShareable(new TArray<FGravRacerMenuItem>());
	}

	/** Custom widgets cannot contain sub menus, all functionality must be handled by custom widget itself */
	FGravRacerMenuItem(TSharedPtr<SWidget> _Widget)
	{
		MenuItemType = EGravRacerMenuItemType::CustomWidget;
		CustomWidget = _Widget;
	}

};

namespace MenuHelper
{
	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCall(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FGravRacerMenuItem::FOnConfirmMenuItem::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		if (World->IsPaused())
		{
			// just call it right away
			FTimerDelegate::CreateUObject(inObj, inMethod).Execute();
		}
		else
		{
			// timers don't run when game is paused
			const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
			World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateUObject(inObj, inMethod), SoundDuration, false);
		}

		return Result;
	}

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCallSP(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FGravRacerMenuItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateSP(inObj, inMethod), SoundDuration, false);

		return Result;
	}

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCallSP(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FGravRacerMenuItem::FOnConfirmMenuItem::TSPMethodDelegate_Const< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateSP(inObj, inMethod), SoundDuration, false);

		return Result;
	}

	FORCEINLINE float GetSoundPlaybackPosition(UWorld* World, const FSlateSound& Sound, FTimerHandle inHandle)
	{
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		return World->GetTimerManager().GetTimerElapsed(inHandle) / SoundDuration;
	}
}
