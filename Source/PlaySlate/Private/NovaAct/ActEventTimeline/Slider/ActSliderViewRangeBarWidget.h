#pragma once
#include "Common/NovaStruct.h"

class SActSliderWidget;

class SActSliderViewRangeBarWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActSliderViewRangeBarWidget) { }
	SLATE_END_ARGS()

	SActSliderViewRangeBarWidget();
	virtual ~SActSliderViewRangeBarWidget() override;

	void Construct(const FArguments& InArgs);

	//~Begin SWidget interface
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	//~End SWidget interface

	/** 计算拖拽部分的offset距离 */
	void ComputeHandleOffsets(float& LeftHandleOffset, float& RightHandleOffset, float& HandleOffset, int32 GeometryWidth) const;
	/**
	 * 计算单位拖拽的时间跨度
	 *
	 * @param MouseEvent
	 * @param GeometryWidth
	 * @return DragDelta 拖动的距离转换成 时间
	 */
	double ComputeDragDelta(const FPointerEvent& MouseEvent, float GeometryWidth) const;
	/** 重置所有状态 */
	void ResetState();
	/** 重置按下中的状态 */
	void ResetHoveredState();

protected:
	/** TimeSlider ViewRange记录 */
	// TRange<float> LastViewRange;
	/** 拖拽中 */
	bool bHandleDragged;
	/** 左侧拖拽中 */
	bool bLeftHandleDragged;
	/** 右侧拖拽中 */
	bool bRightHandleDragged;
	/** 按下中 */
	bool bHandleHovered;
	/** 左侧按下中 */
	bool bLeftHandleHovered;
	/** 右侧按下中 */
	bool bRightHandleHovered;
	/* The position of the mouse on mouse down */
	FVector2D MouseDownPosition;
	/* The in/out view range on mouse down */
	TRange<double> MouseDownViewRange;
	/** 拖拽按钮单个的宽度 */
	float HandleSize = 14.0f;
};
