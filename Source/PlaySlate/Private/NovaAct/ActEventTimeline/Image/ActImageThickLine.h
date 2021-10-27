#pragma once

#include "Common/NovaStruct.h"

class SActImageScrubPosition;
class SActSliderWidget;

using namespace NovaDelegate;
using namespace NovaStruct;

class SActImageThickLine : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActImageThickLine) { }
	SLATE_END_ARGS()

	virtual ~SActImageThickLine() override;

	void Construct(const FArguments& InArgs);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface
};
