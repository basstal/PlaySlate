#pragma once
#include "Common/NovaEnum.h"

using namespace NovaEnum;

class SActImageTreeViewTableRow;

class SActImageTrackPanel : public SCompoundWidget
{
public:
	// ** 使用 TPanelChildren 必须有的结构
	class Slot : public TSlotBase<Slot>
	{
	public:
		Slot(const TSharedRef<SActImageTrackPanel>& InSlotContent);

		/** @return Get the vertical position of this slot inside its parent. */
		float GetVerticalOffset() const;

		/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Horizontal alignment for the slot. */
		EHorizontalAlignment HAlignment;
		/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Vertical alignment for the slot. */
		EVerticalAlignment VAlignment;

	protected:
		/** Slot 的具体 Widget 内容，这里是WeakPtr是因为基类有对 Widget 的管理 */
		TSharedPtr<SActImageTrackPanel> SlotContent;
	};
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

	// void Update();

	/** Get the desired physical vertical position of this track */
	float GetPhysicalPosition() const;
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
	TSharedPtr<SActImageTreeViewTableRow> ActImageTreeViewTableRow;
};
