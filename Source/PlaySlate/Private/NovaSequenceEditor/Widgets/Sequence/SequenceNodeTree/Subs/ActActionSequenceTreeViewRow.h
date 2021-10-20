#pragma once

#include "Utils/ActActionDelegates.h"

class FActActionSequenceTreeViewNode;

class SActActionSequenceTrackLane;

class SActActionSequenceTreeViewRow : public SMultiColumnTableRow<TSharedRef<FActActionSequenceTreeViewNode>>
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTreeViewRow)
		{
		}

		/** Delegate to invoke to create a new column for this row */
		SLATE_EVENT(ActActionSequence::OnGenerateWidgetForColumnDelegate, OnGenerateWidgetForColumn)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<FActActionSequenceTreeViewNode>& InNode);

	//~Begin SMultiColumnTableRow interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~End SMultiColumnTableRow interface

	void AddTrackAreaReference(const TSharedRef<SActActionSequenceTrackLane>& Lane);

	TOptional<EItemDropZone> OnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceTreeViewNode> DisplayNode);

	FReply OnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceTreeViewNode> DisplayNode);

	/** Gets the padding for this row based on whether it is a root node or not */
	FMargin GetRowPadding() const;


protected:
	/** The item associated with this row of data */
	TWeakPtr<FActActionSequenceTreeViewNode> Node;

	/** Delegate to call to create a new widget for a particular column. */
	ActActionSequence::OnGenerateWidgetForColumnDelegate OnGenerateWidgetForColumn;

	/** 对应的TrackLane Widget */ 
	TWeakPtr<SActActionSequenceTrackLane> TargetTrackLane;
};
