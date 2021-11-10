#include "ActActionSequenceSection.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/ActActionSequenceSectionBase.h"

SActActionSequenceSection::SActActionSequenceSection()
	: SectionIndex(0)
{
}

SActActionSequenceSection::~SActActionSequenceSection()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActActionSequenceSection::~SActActionSequenceSection"));
}

void SActActionSequenceSection::Construct(const FArguments& InArgs, TSharedRef<SActImageTreeViewTableRow> SectionNode, int32 InSectionIndex)
{
	SectionIndex = InSectionIndex;
	ParentSectionArea = SectionNode;
	Section = SectionNode->GetSections()[InSectionIndex];

	ChildSlot
	[
		Section->GenerateSectionWidget()
	];
}
