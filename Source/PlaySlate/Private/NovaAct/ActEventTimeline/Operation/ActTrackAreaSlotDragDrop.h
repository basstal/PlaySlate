#pragma once

class SActNotifyPoolNotifyNodeWidget;
struct FTrackScaleInfo;

class FActTrackAreaSlotDragDrop : public FDragDropOperation
{
	// /** FActTrackAreaSlotDragDrop 使用 */
	// struct FNovaTrackClampInfo
	// {
	// 	int32 TrackPos;
	// 	int32 TrackSnapTestPos;
	// 	TSharedPtr<SActNotifyPoolNotifyNodeWidget> Notify;
	// };
public:
	FActTrackAreaSlotDragDrop(float& InCurrentDragXPosition);

	DRAG_DROP_OPERATOR_TYPE(FNotifyDragDropOp, FDragDropOperation)

	//~Begin FDragDropOperation interface
	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;
	//~End FDragDropOperation interface

	// FNovaTrackClampInfo& GetTrackClampInfo(const FVector2D NodePos);

	static TSharedRef<FActTrackAreaSlotDragDrop> New(TSharedRef<SActNotifyPoolNotifyNodeWidget> NotifyNode,
	                                                 TSharedPtr<SWidget> Decorator,
	                                                 const FVector2D& CursorPosition,
	                                                 const FVector2D& SelectionScreenPosition,
	                                                 const FVector2D& SelectionSize,
	                                                 float& CurrentDragXPosition);
	/** TODO: */
	float GetSnapPosition(const FTrackScaleInfo& InScaleInfo, float WidgetSpaceNotifyPosition, bool& bOutSnapped, float PlayLength);

protected:
	float CurrentDragXPosition;                           // Store the position of a currently dragged node for display across tracks
	float SnapTime;                                       // The time that the snapped node was snapped to
	float SelectionTimeLength;                            // Length of time that the selection covers
	FVector2D NodeGroupPosition;                          // Position of the beginning of the selection
	FVector2D NodeGroupSize;                              // Size of the entire selection
	FVector2D DragOffset;                                 // Offset from the mouse to place the decorator
	TSharedPtr<SWidget> Decorator;                        // The widget to display when dragging
	TSharedPtr<SActNotifyPoolNotifyNodeWidget> SelectedNode;// The node that are in the current selection
	int32 TrackSpan;                                      // Number of tracks that the selection spans
	float NodeTimeOffset;                                 // Time offset from the beginning of the selection to the nodes.
	float NodeTime;                                       // Time to drop each selected node at
	float NodeXOffset;                                    // Offsets in X from the widget position to the scrub handle for each node.
};
