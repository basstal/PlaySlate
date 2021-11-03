#include "ActTrackPanelFolderWidget.h"

#include "Common/NovaConst.h"

using namespace NovaConst;

SActTrackPanelFolderWidget::SActTrackPanelFolderWidget()
	: bIsVisible(false),
	  Padding(0.0f),
	  Height(NotifyHeight),
	  bIsHovered(false) {}

void SActTrackPanelFolderWidget::Construct(const FArguments& InArgs) { }

int32 SActTrackPanelFolderWidget::OnPaint(const FPaintArgs& Args,
                                          const FGeometry& AllottedGeometry,
                                          const FSlateRect& MyCullingRect,
                                          FSlateWindowElementList& OutDrawElements,
                                          int32 LayerId,
                                          const FWidgetStyle& InWidgetStyle,
                                          bool bParentEnabled) const
{
	static const FName BorderName("AnimTimeline.Outliner.DefaultBorder");

	float TotalNodeHeight = Height + Padding.Combined();

	// draw hovered
	if (bIsHovered)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId++,
			AllottedGeometry.ToPaintGeometry(
				FVector2D(0, 0),
				FVector2D(AllottedGeometry.GetLocalSize().X, TotalNodeHeight)
			),
			FEditorStyle::GetBrush(BorderName),
			ESlateDrawEffect::None,
			FLinearColor::White.CopyWithNewOpacity(0.05f)
		);
	}

	// Draw track bottom border
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		TArray<FVector2D>({
			FVector2D(0.0f, TotalNodeHeight),
			FVector2D(AllottedGeometry.GetLocalSize().X, TotalNodeHeight)
		}),
		ESlateDrawEffect::None,
		FLinearColor::White.CopyWithNewOpacity(0.3f)
	);
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 1, InWidgetStyle, bParentEnabled);
}

FVector2D SActTrackPanelFolderWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100.0f, Height);
}
