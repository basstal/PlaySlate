#pragma once

class FActActionSequenceTreeViewNode;
class FActActionSequenceSectionBase;
class SActActionSequenceSection;

class SActActionSequenceSectionArea : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceSectionArea)
		{
		}

		/** The view range of the section area */
		SLATE_ATTRIBUTE(TRange<double>, ViewRange)
	SLATE_END_ARGS()

	SActActionSequenceSectionArea(): Children(this)
	{
	}

	void Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceTreeViewNode> Node);
	EVisibility GetSectionVisibility(UObject* SectionObject) const;
	bool GetSectionEnabled(TSharedRef<FActActionSequenceSectionBase> InSequenceSection) const;
	FText GetSectionToolTip(TSharedRef<FActActionSequenceSectionBase> InSequenceSection) const;

	//~Being SWidget interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SWidget interface

protected:
	/** The node containing the sections we are viewing/manipulating */
	TSharedPtr<FActActionSequenceTreeViewNode> SectionAreaNode;
	/** The current view range */
	TAttribute<TRange<double>> ViewRange;
	/** All the widgets in the panel */
	TSlotlessChildren<SWidget> Children;
};
