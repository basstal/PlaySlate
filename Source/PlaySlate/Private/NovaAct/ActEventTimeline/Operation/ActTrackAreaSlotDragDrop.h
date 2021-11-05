#pragma once
#include "Common/NovaStruct.h"

class SActNotifyPoolNotifyNodeWidget;
struct FTrackScaleInfo;
class SActPoolWidgetNotifyWidget;

using namespace NovaStruct;

class FActTrackAreaSlotDragDrop : public FDragDropOperation
{
public:
	FActTrackAreaSlotDragDrop(float& InCurrentDragXPosition);

	DRAG_DROP_OPERATOR_TYPE(FNotifyDragDropOp, FDragDropOperation)

	//~Begin FDragDropOperation interface
	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;
	//~End FDragDropOperation interface

	/**
	 * 构造一个 DragDrop 的上下文类型
	 * 
	 * @param PoolNotifyWidget
	 * @param NotifyNodes
	 * @param Decorator
	 * @param CursorPosition
	 * @param SelectionScreenPosition
	 * @param SelectionSize
	 * @param CurrentDragXPosition
	 * @return 
	 */
	static TSharedRef<FActTrackAreaSlotDragDrop> New(
		TSharedPtr<SActPoolWidgetNotifyWidget> PoolNotifyWidget,
		TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>> NotifyNodes,
		TSharedPtr<SWidget> Decorator,
		const FVector2D& CursorPosition,
		const FVector2D& SelectionScreenPosition,
		const FVector2D& SelectionSize,
		float& CurrentDragXPosition);

	/**
	 * 获得 Snap 功能的具体位置
	 *
	 * @param InClampInfo
	 * @param WidgetSpaceNotifyPosition
	 * @param bOutSnapped
	 */
	float GetSnapPosition(const TSharedRef<FActDragDropLaneClampInfo>& InClampInfo,
	                      float WidgetSpaceNotifyPosition,
	                      float PlayLength,
	                      bool& bOutSnapped);
	/**
	 * 获得与指定位置最近能够 Snap 的 Lane ClampInfo
	 *
	 * @param NodePos 指定位置
	 */
	TSharedPtr<FActDragDropLaneClampInfo> GetLaneClampInfo(FVector2D NodePos);

protected:
	// Store the position of a currently dragged node for display across tracks
	float& CurrentDragXPosition;
	// Position of the beginning of the selection
	FVector2D NodeGroupPosition;
	// Size of the entire selection
	FVector2D NodeGroupSize;
	// Offset from the mouse to place the decorator
	FVector2D DragOffset;
	// The widget to display when dragging
	TSharedPtr<SWidget> Decorator;
	// The node that are in the current selection
	TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>> SelectedNodes;
	// Number of tracks that the selection spans
	int32 TrackSpan;
	// Time offset from the beginning of the selection to the nodes.
	TArray<float> NodesTimeOffset;
	// Offsets in X from the widget position to the scrub handle for each node.
	TArray<float> NodesXOffset;
	// Clamping information for all of the available tracks
	TArray<TSharedPtr<FActDragDropLaneClampInfo>> ClampInfos;
	// 所有选中节点中位于最前面的，即 AnimNotifyEvent 触发 Time 最小的
	TSharedPtr<SActNotifyPoolNotifyNodeWidget> BeginSelectedNode;
};
