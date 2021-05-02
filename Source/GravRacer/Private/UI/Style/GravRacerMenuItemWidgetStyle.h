// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "GravRacerMenuItemWidgetStyle.generated.h"

/**
 * Represents the the appearance of an SGravRacerMenuItem
 */
USTRUCT()
struct FGravRacerMenuItemStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FGravRacerMenuItemStyle();
	virtual ~FGravRacerMenuItemStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FGravRacerMenuItemStyle& GetDefault();

	/**
	 * The brush used to draw the menu item background
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush BackgroundBrush;
	FGravRacerMenuItemStyle& SetBackgroundBrush(const FSlateBrush& InBackgroundBrush) { BackgroundBrush = InBackgroundBrush; return *this; }

	/**
	 * The brush used to draw the menu highlight
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush HighlightBrush;
	FGravRacerMenuItemStyle& SetHighlightBrush(const FSlateBrush& InHighlightBrush) { HighlightBrush = InHighlightBrush; return *this; }

	/**
	 * The style used for the controls list table row
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FTableRowStyle ControlsListTableRowStyle;
	FGravRacerMenuItemStyle& SetControlsListTableRowStyle(const FTableRowStyle& InControlsListTableRowStyle) { ControlsListTableRowStyle = InControlsListTableRowStyle; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UGravRacerMenuItemWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the menu item's appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FGravRacerMenuItemStyle MenuItemStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &MenuItemStyle );
	}
};
