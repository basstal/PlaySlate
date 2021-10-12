#pragma once

class FActActionSequenceTreeViewNode;
class SActActionSequenceTreeView;

class SActActionSequenceTrackLane : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTrackLane)
		{
		}
		SLATE_DEFAULT_SLOT(FArguments, Content)
	SLATE_END_ARGS()

	/** Construct this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceTreeViewNode>& InDisplayNode, const TSharedRef<SActActionSequenceTreeView>& InTreeView);
protected:
	TSharedPtr<FActActionSequenceTreeViewNode> DisplayNode;
	TSharedPtr<SActActionSequenceTreeView> TreeView;
};
