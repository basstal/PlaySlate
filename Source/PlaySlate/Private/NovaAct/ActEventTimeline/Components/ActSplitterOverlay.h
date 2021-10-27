#pragma once

class SActSplitterOverlay : public SOverlay
{
public:
	// ** 用于把自身参数转换为SSplitter的参数，使其可以 + SSplitter::Slot
	typedef SSplitter::FArguments FArguments;

	void Construct(const FArguments& InArgs);

	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	//~End SPanel interface

	//~Begin SWidget interface
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	//~End SWidget interface
protected:
	/** 用来确定被分割区块的Widget */
	TSharedPtr<SSplitter> Splitter;
	/** 区间间间隔参数，以及分隔方式 */
	mutable TArray<FMargin> SlotPadding;
};
