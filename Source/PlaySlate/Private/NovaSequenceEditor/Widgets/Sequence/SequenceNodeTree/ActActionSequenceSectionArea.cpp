#include "ActActionSequenceSectionArea.h"

#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceSectionBase.h"
#include "Subs/ActActionSequenceSection.h"


#include "PlaySlate.h"

void SActActionSequenceSectionArea::Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceTreeViewNode> InNode)
{
	check(InNode->GetType() == ActActionSequence::ESequenceNodeType::Track);
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
	UE_LOG(LogActAction, Log, TEXT("GetSectionToolTip TODO"));
	// const UMovieSceneSection* SectionObject = InSequenceSection->GetSectionObject();
	// const UMovieScene* MovieScene = SectionObject ? SectionObject->GetTypedOuter<UMovieScene>() : nullptr;
	//
	// // Optional section specific content to add to tooltip
	// FText SectionToolTipContent = InSequenceSection->GetSectionToolTip();
	//
	// FText SectionTitleText = InSequenceSection->GetSectionTitle();
	// if (!SectionTitleText.IsEmpty())
	// {
	// 	SectionTitleText = FText::Format(FText::FromString(TEXT("{0}\n")), SectionTitleText);
	// }
	//
	// // If the objects are valid and the section is not unbounded, add frame information to the tooltip
	// if (SectionObject && MovieScene && SectionObject->HasStartFrame() && SectionObject->HasEndFrame())
	// {
	// 	int32 StartFrame = ConvertFrameTime(SectionObject->GetInclusiveStartFrame(), MovieScene->GetTickResolution(), MovieScene->GetDisplayRate()).RoundToFrame().Value;
	// 	int32 EndFrame = ConvertFrameTime(SectionObject->GetExclusiveEndFrame(), MovieScene->GetTickResolution(), MovieScene->GetDisplayRate()).RoundToFrame().Value;
	//
	// 	if (SectionToolTipContent.IsEmpty())
	// 	{
	// 		return FText::Format(NSLOCTEXT("SequencerSection", "TooltipFormat", "{0}{1} - {2} ({3} frames)"), SectionTitleText,
	// 			StartFrame,
	// 			EndFrame,
	// 			EndFrame - StartFrame);
	// 	}
	// 	else
	// 	{
	// 		return FText::Format(NSLOCTEXT("SequencerSection", "TooltipFormatWithSectionContent", "{0}{1} - {2} ({3} frames)\n{4}"), SectionTitleText,
	// 			StartFrame,
	// 			EndFrame,
	// 			EndFrame - StartFrame,
	// 			SectionToolTipContent);
	// 	}
	// }
	// else
	// {
	// 	if (SectionToolTipContent.IsEmpty())
	// 	{
	// 		return InSequencerSection->GetSectionTitle();
	// 	}
	// 	else
	// 	{
	// 		return FText::Format(NSLOCTEXT("SequencerSection", "TooltipSectionContentFormat", "{0}{1}"), SectionTitleText, SectionToolTipContent);
	// 	}
	// }
	return FText(NSLOCTEXT("SequencerSection", "TooltipSectionContentFormat", "111"));
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
