#include "ActActionSequenceSection.h"

#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceSectionBase.h"

void SActActionSequenceSection::Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceTreeViewNode> SectionNode, int32 InSectionIndex)
{
	SectionIndex = InSectionIndex;
	ParentSectionArea = SectionNode;
	Section = SectionNode->GetSections()[InSectionIndex];
	// Layout = FSectionLayout(*SectionNode, InSectionIndex);
	// HandleOffsetPx = 0.f;

	ChildSlot
	[
		Section->GenerateSectionWidget()
	];
}
