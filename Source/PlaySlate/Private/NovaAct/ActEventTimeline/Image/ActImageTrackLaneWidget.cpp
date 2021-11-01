#include "ActImageTrackLaneWidget.h"

// #include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
// #include "NovaAct/ActEventTimeline/Image/ActImageTrackCarWidget.h"

#include "Widgets/SWeakWidget.h"

SActImageTrackLaneWidget::Slot::Slot(const TSharedRef<SActImageTrackLaneWidget>& InSlotContent)
{
	SlotContent = InSlotContent;

	HAlignment = HAlign_Fill;
	VAlignment = VAlign_Top;

	AttachWidget(
		SNew(SWeakWidget)
		.Clipping(EWidgetClipping::ClipToBounds)
		.PossiblyNullContent(InSlotContent)
	);
}

float SActImageTrackLaneWidget::Slot::GetVerticalOffset() const
{
	if (SlotContent.IsValid())
	{
		return SlotContent->GetPhysicalPosition();
	}
	return 0.0f;
}

void SActImageTrackLaneWidget::Construct(const FArguments& InArgs, const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow)
{
	Height = 24.0f;

	ActImageTreeViewTableRow = InActImageTreeViewTableRow;
}

int32 SActImageTrackLaneWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// static const FName BorderName("AnimTimeline.Outliner.DefaultBorder");
	// static const FName SelectionColorName("SelectionColor");

	// TSharedPtr<FAnimTimelineTrack> Track = WeakTrack.Pin();
	// if(Track.IsValid())
	// {
	// 	if (Track->IsVisible())
	// 	{
	// float TotalNodeHeight = Track->GetHeight() + Track->GetPadding().Combined();
	//
	// // draw hovered
	// if (Track->IsHovered())
	// {
	// 	FSlateDrawElement::MakeBox(
	// 		OutDrawElements,
	// 		LayerId++,
	// 		AllottedGeometry.ToPaintGeometry(
	// 			FVector2D(0, 0),
	// 			FVector2D(AllottedGeometry.GetLocalSize().X, TotalNodeHeight)
	// 		),
	// 		FEditorStyle::GetBrush(BorderName),
	// 		ESlateDrawEffect::None,
	// 		FLinearColor(1.0f, 1.0f, 1.0f, 0.05f)
	// 	);
	// }

	// Draw track bottom border
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		TArray<FVector2D>({
			FVector2D(0.0f, Height),
			FVector2D(AllottedGeometry.GetLocalSize().X, Height)
		}),
		ESlateDrawEffect::None,
		FLinearColor::White.CopyWithNewOpacity(0.2f)
	);
	// }
	//
	// }

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 1, InWidgetStyle, bParentEnabled);
}

FVector2D SActImageTrackLaneWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100.0f, Height);
}

// void SActImageTrackLaneWidget::MakeTrackLane()
// {
// 	// TrackLane = SNew(SActImageTrackCarWidget)
// 	// [
// 	// 	ActImageTreeViewTableRow.Pin()->GetActActionSectionWidget()
// 	// ];
// 	// TSharedRef<SWeakWidget> AttachedWidget = SNew(SWeakWidget)
// 	// .Clipping(EWidgetClipping::ClipToBounds)
// 	// .PossiblyNullContent(TrackLane);
// 	// AttachWidget(AttachedWidget);
// }


FText SActImageTrackLaneWidget::GetNodeTooltip()
{
	return FText();
}

float SActImageTrackLaneWidget::GetPlayLength()
{
	// FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
	// ** TODO;
	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
	return 0;
}

FName SActImageTrackLaneWidget::GetName()
{
	return NAME_None;
}

FLinearColor SActImageTrackLaneWidget::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}

//
// void SActImageTrackLaneWidget::GetTime(float& OutTime, int32& OutFrame)
// {
// 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// 	OutTime = 0;
// 	OutFrame = 0;
// 	if (ActImageTreeViewTableRow.Pin()->GetType() == ENovaTreeViewNodeType::State)
// 	{
// 		OutTime = FMath::Max(OutTime, (float)(TrackAreaArgs.Begin.Get() * FrameRate.AsInterval()));
// 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.Begin.Get());
// 	}
// }
//
// void SActImageTrackLaneWidget::GetDuration(float& OutTime, int32& OutFrame)
// {
// 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// 	OutTime = NovaConst::ActMinimumNotifyStateFrame * FrameRate.AsInterval();
// 	OutFrame = NovaConst::ActMinimumNotifyStateFrame;
// 	if (ActImageTreeViewTableRow.Pin()->GetType() == ENovaTreeViewNodeType::State)
// 	{
// 		OutTime = FMath::Max(OutTime, (float)((TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get()) * FrameRate.AsInterval()));
// 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get());
// 	}
// }

bool SActImageTrackLaneWidget::IsBranchingPoint()
{
	return true;
}

bool SActImageTrackLaneWidget::HasNotifyNode()
{
	// ENovaTreeViewNodeType NodeType = ActImageTreeViewTableRow.Pin()->GetType();
	// return NodeType != ENovaTreeViewNodeType::Root && NodeType != ENovaTreeViewNodeType::Folder;
	return true;
}

// void SActImageTrackLaneWidget::SetVisibility(EVisibility InVisibility)
// {
// 	TrackLane->SetVisibility(InVisibility);
// }


float SActImageTrackLaneWidget::GetPhysicalPosition() const
{
	if (ActImageTreeViewTableRow.IsValid())
	{
		return ActImageTreeViewTableRow.Pin()->ComputeTrackPosition();
	}
	return 0.0f;
}
