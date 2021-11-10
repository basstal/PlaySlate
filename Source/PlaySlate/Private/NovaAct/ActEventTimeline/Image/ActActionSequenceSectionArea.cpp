#include "ActActionSequenceSectionArea.h"

#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/ActActionSequenceSectionBase.h"
#include "Subs/ActActionSequenceSection.h"


#include "PlaySlate.h"

void SActActionSequenceSectionArea::Construct(const FArguments& InArgs, TSharedRef<SActImageTreeViewTableRow> InNode)
{
	SectionAreaNode = InNode;

	// Generate widgets for sections in this view
	Children.Empty();

	if (SectionAreaNode.IsValid())
	{
		TArray<TSharedRef<FActActionSequenceSectionBase>>& Sections = SectionAreaNode->GetSections();

		for (int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex)
		{
			Children.Add(
				SNew(SActActionSequenceSection, SectionAreaNode.ToSharedRef(), SectionIndex)
				.Visibility(this, &SActActionSequenceSectionArea::GetSectionVisibility, Sections[SectionIndex]->GetSectionObject())
				.IsEnabled(this, &SActActionSequenceSectionArea::GetSectionEnabled, Sections[SectionIndex])
				.ToolTipText(this, &SActActionSequenceSectionArea::GetSectionToolTip, Sections[SectionIndex]));
		}
	}
}


EVisibility SActActionSequenceSectionArea::GetSectionVisibility(UObject* SectionObject) const
{
	return EVisibility::Visible;
}


bool SActActionSequenceSectionArea::GetSectionEnabled(TSharedRef<FActActionSequenceSectionBase> InSequenceSection) const
{
	return !InSequenceSection->IsReadOnly();
}


FText SActActionSequenceSectionArea::GetSectionToolTip(TSharedRef<FActActionSequenceSectionBase> InSequenceSection) const
{
	UE_LOG(LogNovaAct, Log, TEXT("GetSectionToolTip TODO"));
	return FText(NSLOCTEXT("SequencerSection", "TooltipSectionContentFormat1", "Default?"));
}

void SActActionSequenceSectionArea::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
}

FVector2D SActActionSequenceSectionArea::ComputeDesiredSize(float) const
{
	// Note: X Size is not used
	FVector2D Size(100, 0.0f);
	if (Children.Num())
	{
		for (int32 Index = 0; Index < Children.Num(); ++Index)
		{
			Size.Y = FMath::Max(Size.Y, Children[Index]->GetDesiredSize().Y);
		}
	}
	else
	{
		Size.Y = SectionAreaNode->GetNodeHeight();
	}
	return Size;
}

FChildren* SActActionSequenceSectionArea::GetChildren()
{
	return &Children;
}
