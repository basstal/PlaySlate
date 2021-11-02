#include "ActImageAreaPanel.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTrackLaneWidget.h"


SActImageAreaPanel::SActImageAreaPanel()
	: Children(this) {}

SActImageAreaPanel::~SActImageAreaPanel()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActImageAreaPanel::~SActImageAreaPanel "));
}

void SActImageAreaPanel::Construct(const FArguments& InArgs) {}

void SActImageAreaPanel::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
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

		AlignmentArrangeResult XResult = AlignChild<Orient_Horizontal>(AllottedGeometry.GetLocalSize().X,
		                                                               CurrentChild,
		                                                               Padding,
		                                                               1.0f,
		                                                               false);
		AlignmentArrangeResult YResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y,
		                                                             CurrentChild,
		                                                             Padding,
		                                                             1.0f,
		                                                             false);

		FArrangedWidget ChildWidget = AllottedGeometry.MakeChild(CurrentChild.GetWidget(),
		                                                         FVector2D(XResult.Offset, YResult.Offset),
		                                                         FVector2D(XResult.Size, YResult.Size));
		ArrangedChildren.AddWidget(ChildVisibility, ChildWidget);
	}
}

FVector2D SActImageAreaPanel::ComputeDesiredSize(float) const
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

FChildren* SActImageAreaPanel::GetChildren()
{
	return &Children;
}

TSharedRef<SActImageTrackLaneWidget> SActImageAreaPanel::MakeTrackLane(const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow)
{
	TSharedRef<SActImageTrackLaneWidget> TrackLaneWidget = SNew(SActImageTrackLaneWidget, InActImageTreeViewTableRow);
	Children.Add(new SActImageTrackLaneWidget::Slot(TrackLaneWidget));
	return TrackLaneWidget;
}
