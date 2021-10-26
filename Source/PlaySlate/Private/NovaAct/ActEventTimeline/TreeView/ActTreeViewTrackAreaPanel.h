#pragma once

class SActTreeViewTrackCarWidget;
class SActTreeViewNode;
class SActTreeViewTrackLaneWidget;

class SActTreeViewTrackAreaPanel : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActTreeViewTrackAreaPanel) { }
	SLATE_END_ARGS()

	SActTreeViewTrackAreaPanel();

	void Construct(const FArguments& InArgs);

	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SPanel interface

	/**
	 * Add a new track slot to this area for the given node. The slot will be automatically cleaned up when all external references to the supplied slot are removed.
	 *
	 * @param InActTreeViewNode
	 * @return
	 */
	TSharedRef<SActTreeViewTrackLaneWidget> MakeTrackLaneWithTreeViewNode(const TSharedRef<SActTreeViewNode>& InActTreeViewNode);

protected:
	TPanelChildren<SActTreeViewTrackLaneWidget> Children;// ** The track area's children.
};
