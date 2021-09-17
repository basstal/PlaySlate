#include "ActActionSequenceTrackArea.h"

FActActionTrackAreaSlot::FActActionTrackAreaSlot()
{
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
