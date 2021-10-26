#pragma once

#include "Common/NovaDelegate.h"

class SActTreeViewNode;
class SActTreeViewTrackCarWidget;

using namespace NovaDelegate;

class SActActionSequenceTreeViewRow : public SMultiColumnTableRow<TSharedRef<SActTreeViewNode>>
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTreeViewRow) { }
		/** Delegate to invoke to create a new column for this row */
		SLATE_EVENT(OnGenerateWidgetForColumnDelegate, OnGenerateWidgetForColumn)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<SActTreeViewNode>& InNode);

	//~Begin SMultiColumnTableRow interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~End SMultiColumnTableRow interface

	// void AddTrackAreaReference(const TSharedRef<SActTreeViewTrackCarWidget>& Lane);

	TOptional<EItemDropZone> OnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<SActTreeViewNode> DisplayNode);

	FReply OnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<SActTreeViewNode> DisplayNode);

	/** Gets the padding for this row based on whether it is a root node or not */
	FMargin GetRowPadding() const;


protected:
	/** The item associated with this row of data */
	TWeakPtr<SActTreeViewNode> Node;

	/** Delegate to call to create a new widget for a particular column. */
	OnGenerateWidgetForColumnDelegate OnGenerateWidgetForColumn;

	/** 对应的TrackLane Widget */
	// TWeakPtr<SActTreeViewTrackCarWidget> TargetTrackLane;
};
