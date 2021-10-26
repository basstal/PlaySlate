#pragma once
#include "ActActionSequenceNotifyNode.h"

class FActActionSequenceTreeViewNode;
class SActActionSequenceTreeView;

class SActActionSequenceTrackLane : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTrackLane)
		{
		}

		SLATE_DEFAULT_SLOT(FArguments, Content)

		SLATE_EVENT(OnGetDraggedNodePosDelegate, OnGetDraggedNodePos)
		SLATE_ARGUMENT(int32, TrackIndex)
		SLATE_ARGUMENT(FLinearColor, TrackColor)
	SLATE_END_ARGS()

	/** Construct this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<FActActionTrackAreaSlot>& InTrackAreaSlot);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	//~End SWidget interface

	/**
	* Update the nodes to match the data that the panel is observing
	*/
	void Update();
	/** TODO: */
	FReply OnNotifyNodeDragStarted(TSharedRef<SActActionSequenceNotifyNode> InNotifyNode, const FPointerEvent& MouseEvent, const FVector2D& ScreenNodePosition, const bool bDragOnMarker);
	/** TODO: */
	FMargin GetNotifyTrackPadding() const;
	/** Get the desired physical vertical position of this track */
	float GetPhysicalPosition() const;
protected:
	/** Controller */
	TWeakPtr<FActActionTrackAreaSlot> ActActionTrackAreaSlot;

	TSharedPtr<SActActionSequenceTreeView> TreeView;

	/** 当前轨道指定的NotifyNode */
	TSharedPtr<SActActionSequenceNotifyNode> NotifyNode;
	/** Cache the SOverlay used to store all this tracks nodes */
	TSharedPtr<SOverlay> NodeSlots;
	/** Target Track Area */
	TSharedPtr<SBorder> TrackArea;
	/** TODO:把这些参数都放到一个统一的数据结构中 */
	TAttribute<FLinearColor> TrackColor;
	/** TODO: */
	int32 TrackIndex;
	/** TODO: */
	OnGetDraggedNodePosDelegate OnGetDraggedNodePos;
	/** Cached for drag drop handling code */
	FGeometry CachedGeometry;
};
