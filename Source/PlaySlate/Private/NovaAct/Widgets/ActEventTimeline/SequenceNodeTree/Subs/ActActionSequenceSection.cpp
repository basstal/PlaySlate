#include "ActActionSequenceSection.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaAct/ActEventTimeline/SequenceNodeTree/ActActionSequenceSectionBase.h"

SActActionSequenceSection::SActActionSequenceSection()
	: SectionIndex(0)
{
}

SActActionSequenceSection::~SActActionSequenceSection()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceSection::~SActActionSequenceSection"));
}

void SActActionSequenceSection::Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceTreeViewNode> SectionNode, int32 InSectionIndex)
{
	SectionIndex = InSectionIndex;
	ParentSectionArea = SectionNode;
	Section = SectionNode->GetSections()[InSectionIndex];
	// Layout = FSectionLayout(*SectionNode, InSectionIndex);
	// HandleOffsetPx = 0.0f;

	ChildSlot
	[
		Section->GenerateSectionWidget()
	];
}
