#include "ActImageTrackAreaPanel.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTrackLaneWidget.h"


SActImageTrackAreaPanel::SActImageTrackAreaPanel()
	: Children(this) {}

SActImageTrackAreaPanel::~SActImageTrackAreaPanel()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActImageTrackAreaPanel::~SActImageTrackAreaPanel "));
}

void SActImageTrackAreaPanel::Construct(const FArguments& InArgs) {}

void SActImageTrackAreaPanel::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const SActImageTrackLaneWidget::Slot& CurrentChild = Children[ChildIndex];

		const EVisibility ChildVisibility = CurrentChild.GetWidget()->GetVisibility();
		if (!ArrangedChildren.Accepts(ChildVisibility))
		{
			continue;
		}

		const FMargin Padding(0, CurrentChild.GetVerticalOffset(), 0, 0);

		AlignmentArrangeResult XResult = AlignChild<Orient_Horizontal>(AllottedGeometry.GetLocalSize().X, CurrentChild, Padding, 1.0f, false);
		AlignmentArrangeResult YResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, CurrentChild, Padding, 1.0f, false);

		FArrangedWidget ChildWidget = AllottedGeometry.MakeChild(CurrentChild.GetWidget(), FVector2D(XResult.Offset, YResult.Offset), FVector2D(XResult.Size, YResult.Size));
		ArrangedChildren.AddWidget(ChildVisibility, ChildWidget);
	}
}

FVector2D SActImageTrackAreaPanel::ComputeDesiredSize(float) const
{
	FVector2D MaxSize(0.0f, 0.0f);
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const SActImageTrackLaneWidget::Slot& CurrentChild = Children[ChildIndex];

		const EVisibility ChildVisibility = CurrentChild.GetWidget()->GetVisibility();
		if (ChildVisibility != EVisibility::Collapsed)
		{
			FVector2D ChildDesiredSize = CurrentChild.GetWidget()->GetDesiredSize();
			MaxSize.X = FMath::Max(MaxSize.X, ChildDesiredSize.X);
			MaxSize.Y = FMath::Max(MaxSize.Y, ChildDesiredSize.Y);
		}
	}

	return MaxSize;
}

FChildren* SActImageTrackAreaPanel::GetChildren()
{
	return &Children;
}

TSharedRef<SActImageTrackLaneWidget> SActImageTrackAreaPanel::MakeTrackLane()
{
	TSharedRef<SActImageTrackLaneWidget> TrackLaneWidget = SNew(SActImageTrackLaneWidget);
	Children.Add(new SActImageTrackLaneWidget::Slot(TrackLaneWidget));
	return TrackLaneWidget;
}
