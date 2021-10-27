#include "ActActionSequenceCombinedKeysTrack.h"

#include "NovaAct/ActEventTimeline/TreeView/ActImageTreeViewTableRow.h"

void SActActionSequenceCombinedKeysTrack::Construct(const FArguments& InArgs, TSharedRef<SActImageTreeViewTableRow> InRootNode)
{
	RootNode = InRootNode;

	ViewRange = InArgs._ViewRange;
	TickResolution = InArgs._TickResolution;
}

int32 SActActionSequenceCombinedKeysTrack::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// if (RootNode->GetSequenceController().GetSequencerSettings()->GetShowCombinedKeyframes())
	// {
	const FVector2D KeyMarkSize = FVector2D(3.f, 21.f);
	for (float KeyPosition : KeyDrawPositions)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(
				FVector2D(
					KeyPosition - FMath::CeilToFloat(KeyMarkSize.X / 2.f),
					FMath::CeilToFloat(AllottedGeometry.GetLocalSize().Y / 2.f - KeyMarkSize.Y / 2.f)
				),
				KeyMarkSize
			),
			FEditorStyle::GetBrush("Sequencer.KeyMark"),
			ESlateDrawEffect::None,
			FLinearColor(1.f, 1.f, 1.f, 1.f)
		);
	}
	return LayerId + 1;
	// }

	// return LayerId;
}

FVector2D SActActionSequenceCombinedKeysTrack::ComputeDesiredSize(float) const
{
	// Note: X Size is not used
	return FVector2D(100.0f, RootNode->GetNodeHeight());
}
