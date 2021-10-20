#include "ActActionSequenceTrackArea.h"

#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"


SActActionSequenceTrackArea::SActActionSequenceTrackArea()
	: Children(this)
{
}

void SActActionSequenceTrackArea::Construct(const FArguments& InArgs)
{
}

void SActActionSequenceTrackArea::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const FActActionTrackAreaSlot& CurChild = Children[ChildIndex];

		const EVisibility ChildVisibility = CurChild.GetWidget()->GetVisibility();
		if (!ArrangedChildren.Accepts(ChildVisibility))
		{
			continue;
		}

		const FMargin Padding(0, CurChild.GetVerticalOffset(), 0, 0);

		AlignmentArrangeResult XResult = AlignChild<Orient_Horizontal>(AllottedGeometry.GetLocalSize().X, CurChild, Padding, 1.0f, false);
		AlignmentArrangeResult YResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, CurChild, Padding, 1.0f, false);

		FArrangedWidget ChildWidget = AllottedGeometry.MakeChild(CurChild.GetWidget(), FVector2D(XResult.Offset, YResult.Offset), FVector2D(XResult.Size, YResult.Size));
		ArrangedChildren.AddWidget(ChildVisibility, ChildWidget);
	}
}

FVector2D SActActionSequenceTrackArea::ComputeDesiredSize(float) const
{
	FVector2D MaxSize(0.0f, 0.0f);
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const FActActionTrackAreaSlot& CurChild = Children[ChildIndex];

		const EVisibility ChildVisibility = CurChild.GetWidget()->GetVisibility();
		if (ChildVisibility != EVisibility::Collapsed)
		{
			FVector2D ChildDesiredSize = CurChild.GetWidget()->GetDesiredSize();
			MaxSize.X = FMath::Max(MaxSize.X, ChildDesiredSize.X);
			MaxSize.Y = FMath::Max(MaxSize.Y, ChildDesiredSize.Y);
		}
	}

	return MaxSize;
}

FChildren* SActActionSequenceTrackArea::GetChildren()
{
	return &Children;
}

TSharedPtr<SActActionSequenceTrackLane> SActActionSequenceTrackArea::FindTrackSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InNode) const
{
	return TrackSlots.FindRef(InNode).Pin();
}

TSharedRef<SActActionSequenceTrackLane> SActActionSequenceTrackArea::AddTrackSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InNode)
{
	const TSharedRef<SActActionSequenceTrackLane>& InSlot = InNode->GetActActionTrackAreaSlot()->GetActActionSequenceTrackLane();
	TrackSlots.Add(InNode, InSlot);
	Children.Add(&InNode->GetActActionTrackAreaSlot().Get());
	return InSlot;
}
