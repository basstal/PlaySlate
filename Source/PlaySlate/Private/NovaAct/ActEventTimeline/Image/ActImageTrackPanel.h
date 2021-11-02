#pragma once
#include "Common/NovaEnum.h"

using namespace NovaEnum;

class SActImageTreeViewTableRow;

class SActImageTrackPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActImageTrackPanel) {}
		// : _ViewInputMin()
		// , _ViewInputMax()
		// , _InputMin()
		// , _InputMax()
		// , _OnSetInputViewRange()

		//
		// SLATE_ARGUMENT( float, WidgetWidth )
		// SLATE_ATTRIBUTE( float, ViewInputMin )
		// SLATE_ATTRIBUTE( float, ViewInputMax )
		// SLATE_ATTRIBUTE( float, InputMin )
		// SLATE_ATTRIBUTE( float, InputMax )
		// SLATE_EVENT( FOnSetInputViewRange, OnSetInputViewRange )
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow);
	//
	// void PanInputViewRange(int32 ScreenDelta, FVector2D ScreenViewSize);
	//
	// virtual float GetSequenceLength() const { return 0.0f; }

	void Update();
protected:

	// ENovaTreeViewTableRowType TableRowType;
	// // Calls out to notify of a range change, can be overridden by derived classes to respond
	// // but they must call this version too after processing range changes
	// virtual void InputViewRangeChanged(float ViewMin, float ViewMax);
	//
	// /** Create a 2 column widget */
	// TSharedRef<class S2ColumnWidget> Create2ColumnWidget(TSharedRef<SVerticalBox> Parent);

	// TAttribute<float> ViewInputMin;
	// TAttribute<float> ViewInputMax;
	// TAttribute<float> InputMin;
	// TAttribute<float> InputMax;
	// FOnSetInputViewRange OnSetInputViewRange;
	//
	// /** Controls the width of the tracks column */
	// float WidgetWidth;

	TSharedPtr<SWidget> ChildPanel;
	TSharedRef<SActImageTreeViewTableRow> ActImageTreeViewTableRow;
};
