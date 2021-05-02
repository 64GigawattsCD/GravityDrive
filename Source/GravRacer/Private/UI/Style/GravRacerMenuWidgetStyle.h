// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "GravRacerMenuWidgetStyle.generated.h"

/**
 * Represents the the appearance of an SGravRacerMenuWidget
 */
USTRUCT()
struct FGravRacerMenuStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FGravRacerMenuStyle();
	virtual ~FGravRacerMenuStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FGravRacerMenuStyle& GetDefault();

	/**
	 * The sound that should play when entering a sub-menu
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuEnterSound;
	FGravRacerMenuStyle& SetMenuEnterSound(const FSlateSound& InMenuEnterSound) { MenuEnterSound = InMenuEnterSound; return *this; }

	/**
	 * The sound that should play when leaving a sub- menu
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuExitSound;
	FGravRacerMenuStyle& SetMenuExitSound(const FSlateSound& InMenuExitSound) { MenuExitSound = InMenuExitSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UGravRacerMenuWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the menu's appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FGravRacerMenuStyle MenuStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &MenuStyle );
	}
};
