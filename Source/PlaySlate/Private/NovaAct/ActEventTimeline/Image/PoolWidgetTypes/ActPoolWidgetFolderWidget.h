#pragma once
#include "Common/NovaStruct.h"

class FActImageTrackFolder;

using namespace NovaStruct;

class SActPoolWidgetFolderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActPoolWidgetFolderWidget) {}

		// SLATE_DEFAULT_SLOT(FArguments, Content)
		//
		// SLATE_ATTRIBUTE(FAnimatedRange, ViewRange)

	SLATE_END_ARGS()

	SActPoolWidgetFolderWidget();
	void Construct(const FArguments& InArgs, const TSharedRef<FActImageTrackFolder>& InActImageTrackFolder);

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
	/** Is this widget hovered? */
	bool bIsHovered : 1;
	TSharedPtr<FActImageTrackFolder> ActImageTrackFolder;
};
