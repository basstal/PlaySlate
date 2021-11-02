#include "ActImageTrackCarWidget.h"

#include "SCurveEditor.h"
#include "Common/NovaConst.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTrackLaneWidget.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActActionSequenceNotifyNode.h"


void SActImageTrackCarWidget::Construct(const FArguments& InArgs)
{
	SetClipping(EWidgetClipping::ClipToBounds);
	TrackIndex = InArgs._TrackIndex;
	OnGetDraggedNodePos = InArgs._OnGetDraggedNodePos;
	TrackColor = InArgs._TrackColor;

	ChildSlot
	[
		SAssignNew(TrackArea, SBorder)
			.Visibility(EVisibility::SelfHitTestInvisible)
			.BorderImage(FEditorStyle::GetBrush("NoBorder"))
			.Padding(FMargin(0.f, 0.f))
	];

	Update();
}

int32 SActImageTrackCarWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintGeometry MyGeometry = AllottedGeometry.ToPaintGeometry();
	int32 CustomLayerId = LayerId + 1;
	bool bAnyDraggedNodes = false;
	if (NotifyNode)
	{
		if (!NotifyNode.Get()->GetBeingDragged())
		{
			NotifyNode.Get()->UpdateSizeAndPosition(AllottedGeometry);
		}
		else
		{
			bAnyDraggedNodes = true;
		}
	}
	// Draw track bottom border
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		CustomLayerId,
		AllottedGeometry.ToPaintGeometry(),
		TArray<FVector2D>({FVector2D(0.0f, AllottedGeometry.GetLocalSize().Y), FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)}),
		ESlateDrawEffect::None,
		FLinearColor(0.1f, 0.1f, 0.1f, 0.3f)
	);
	++CustomLayerId;

	if (bAnyDraggedNodes && OnGetDraggedNodePos.IsBound())
	{
		float Value = OnGetDraggedNodePos.Execute();

		if (Value >= 0.0f)
		{
			float XPos = Value;
			TArray<FVector2D> LinePoints;
			LinePoints.Add(FVector2D(XPos, 0.f));
			LinePoints.Add(FVector2D(XPos, AllottedGeometry.Size.Y));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				CustomLayerId,
				MyGeometry,
				LinePoints,
				ESlateDrawEffect::None,
				FLinearColor(1.0f, 0.5f, 0.0f)
			);
		}
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, CustomLayerId, InWidgetStyle, bParentEnabled);
}

FVector2D SActImageTrackCarWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	FVector2D Size;
	Size.X = 200;
	Size.Y = NovaConst::NotifyHeight;
	return Size;
}

void SActImageTrackCarWidget::Update()
{
	// TrackArea->SetContent(
	// 	SAssignNew(NodeSlots, SOverlay)
	// );
	//
	// // if (ActNotifiesPanelLaneWidget.IsValid() && ActNotifiesPanelLaneWidget.Pin()->GetNotifyEvent())
	// if (ActActionTrackAreaSlot.IsValid() && ActActionTrackAreaSlot.Pin()->HasNotifyNode())
	// {
	// 	NotifyNode = SNew(SActActionSequenceNotifyNode, ActActionTrackAreaSlot.Pin().ToSharedRef())
	// 		.OnNodeDragStarted(this, &SActImageTrackCarWidget::OnNotifyNodeDragStarted);
	//
	// 	// .OnNotifyStateHandleBeingDragged(OnNotifyStateHandleBeingDragged)
	// 	// .OnUpdatePanel(OnUpdatePanel)
	// 	// .PanTrackRequest(OnRequestTrackPan)
	// 	// .ViewInputMin(ViewInputMin)
	// 	// .ViewInputMax(ViewInputMax)
	// 	// .OnSnapPosition(OnSnapPosition)
	// 	// .OnSelectionChanged(OnSelectionChanged)
	// 	// .StateEndTimingNode(EndTimingNode);
	//
	// 	NodeSlots->AddSlot()
	// 	         .Padding(TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateSP(this, &SActImageTrackCarWidget::GetNotifyTrackPadding)))
	// 	[
	// 		NotifyNode.ToSharedRef()
	// 	];
	// }
}

FReply SActImageTrackCarWidget::OnNotifyNodeDragStarted(TSharedRef<SActActionSequenceNotifyNode> InNotifyNode, const FPointerEvent& MouseEvent, const FVector2D& ScreenNodePosition, const bool bDragOnMarker)
{
	return FReply::Handled().CaptureMouse(InNotifyNode).UseHighPrecisionMouseMovement(InNotifyNode);
}

// Returns the padding needed to render the notify in the correct track position
FMargin SActImageTrackCarWidget::GetNotifyTrackPadding() const
{
	float LeftMargin = NotifyNode->GetWidgetPosition().X;
	float RightMargin = CachedGeometry.GetLocalSize().X - NotifyNode->GetWidgetPosition().X - NotifyNode->ComputeDesiredSize(0).X;
	return FMargin(LeftMargin, 0, RightMargin, 0);
}

const TArray<int32>& SActImageTrackCarWidget::GetSelectedNotifyIndices() const
{
	return SelectedNodeIndices;
}

TSharedRef<FActImageTrackCarNotifyNode> SActImageTrackCarWidget::GetActImageTrackCarNotifyNode() const
{
	return ActImageTrackCarNotifyNode.ToSharedRef();
}
