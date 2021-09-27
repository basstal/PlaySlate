#include "ActActionSequenceTrackArea.h"

#include "ActActionSequenceTrackLane.h"
#include "Editor/ActActionSequenceDisplayNode.h"

FActActionTrackAreaSlot::FActActionTrackAreaSlot(const TSharedRef<SActActionSequenceTrackLane>& InSlotContent)
	: HAlignment(), VAlignment(), TrackLane(InSlotContent)
{
}

float FActActionTrackAreaSlot::GetVerticalOffset() const
{
	// ** TODO:
	return 0.0f;
}

SActActionSequenceTrackArea::SActActionSequenceTrackArea() : Children(this)
{
}

void SActActionSequenceTrackArea::Construct(const FArguments& InArgs)
{
}

void SActActionSequenceTrackArea::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
}

FVector2D SActActionSequenceTrackArea::ComputeDesiredSize(float) const
{
	return FVector2D::ZeroVector;
}

FChildren* SActActionSequenceTrackArea::GetChildren()
{
	return &Children;
}

TSharedPtr<SActActionSequenceTrackLane> SActActionSequenceTrackArea::FindTrackSlot(const TSharedRef<FActActionSequenceDisplayNode>& InNode)
{
	return TrackSlots.FindRef(InNode).Pin();
}

void SActActionSequenceTrackArea::AddTrackSlot(const TSharedRef<FActActionSequenceDisplayNode>& InNode, const TSharedRef<SActActionSequenceTrackLane>& InSlot)
{
	TrackSlots.Add(InNode, InSlot);
	Children.Add(new FActActionTrackAreaSlot(InSlot));
}
