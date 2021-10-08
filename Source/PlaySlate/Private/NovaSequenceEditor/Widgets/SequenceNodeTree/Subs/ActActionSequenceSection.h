#pragma once

class FActActionSequenceDisplayNode;
class FActActionSequenceSectionBase;

class SActActionSequenceSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceSection)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceDisplayNode> SectionNode, int32 InSectionIndex);
protected:
	TSharedPtr<FActActionSequenceSectionBase> Section;
	
	int32 SectionIndex;
	TSharedPtr<FActActionSequenceDisplayNode> ParentSectionArea;
};
