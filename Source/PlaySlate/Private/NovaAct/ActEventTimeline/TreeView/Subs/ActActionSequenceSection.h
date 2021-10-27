#pragma once

class SActImageTreeViewTableRow;
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

	void Construct(const FArguments& InArgs, TSharedRef<SActImageTreeViewTableRow> SectionNode, int32 InSectionIndex);
protected:
	TSharedPtr<FActActionSequenceSectionBase> Section;

	int32 SectionIndex;
	TSharedPtr<SActImageTreeViewTableRow> ParentSectionArea;
};
