#pragma once
#include "Common/NovaStruct.h"

using namespace NovaStruct;

class SActTrackPanelFolderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActTrackPanelFolderWidget) {}

		// SLATE_DEFAULT_SLOT(FArguments, Content)
		//
		// SLATE_ATTRIBUTE(FAnimatedRange, ViewRange)

	SLATE_END_ARGS()

	SActTrackPanelFolderWidget();
	void Construct(const FArguments& InArgs);

	//~Begin SCompoundWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args,
	                      const FGeometry& AllottedGeometry,
	                      const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32 LayerId,
	                      const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	//~End SCompoundWidget interface


protected:
	bool bIsVisible;
	FActImageTrackPadding Padding;
	float Height;
	/** Is this widget hovered? */
	bool bIsHovered : 1;
};
