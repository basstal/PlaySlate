﻿#include "ActActionSequenceSection.h"

void SActActionSequenceSection::Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceDisplayNode> SectionNode, int32 InSectionIndex)
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