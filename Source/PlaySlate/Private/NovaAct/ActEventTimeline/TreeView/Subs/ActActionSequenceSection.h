#pragma once

class SActTreeViewNode;
class FActActionSequenceSectionBase;

class SActActionSequenceSection : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SActActionSequenceSection)
		{
		}

	SLATE_END_ARGS()

	SActActionSequenceSection();
	virtual ~SActActionSequenceSection() override;

	void Construct(const FArguments& InArgs, TSharedRef<SActTreeViewNode> SectionNode, int32 InSectionIndex);
protected:
	TSharedPtr<FActActionSequenceSectionBase> Section;

	int32 SectionIndex;
	TSharedPtr<SActTreeViewNode> ParentSectionArea;
};
