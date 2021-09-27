#pragma once

#include "ActActionSequenceTreeView.h"
#include "Editor/ActActionSequenceDisplayNode.h"

class SActActionSequenceTrackLane : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTrackLane)
		{
		}
		SLATE_DEFAULT_SLOT(FArguments, Content)
	SLATE_END_ARGS()

	/** Construct this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceDisplayNode>& InDisplayNode, const TSharedRef<SActActionSequenceTreeView>& InTreeView);
protected:
	TSharedPtr<FActActionSequenceDisplayNode> DisplayNode;
	TSharedPtr<SActActionSequenceTreeView> TreeView;
};
