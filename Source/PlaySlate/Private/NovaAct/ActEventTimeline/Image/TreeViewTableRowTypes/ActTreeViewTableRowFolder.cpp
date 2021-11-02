#include "ActTreeViewTableRowFolder.h"

#include "PlaySlate.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActTreeViewTableRowFolder::FActTreeViewTableRowFolder()
	: DisplayName(LOCTEXT("NovaAct_TreeViewTableRowFolderDisplayName", "Tree view table row folder display name.")),
	  ToolTipText(LOCTEXT("NovaAct_TreeViewTableRowFolderToolTipText", "Tree view table row folder tool tip text.")),
	  bIsHeaderTableRow(false) {}

FActTreeViewTableRowFolder::~FActTreeViewTableRowFolder()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActTreeViewTableRowFolder::~FActTreeViewTableRowFolder"));
}

TSharedRef<SWidget> FActTreeViewTableRowFolder::GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow)
{
	return SNew(SBorder)
			.ToolTipText_Lambda([=] { return ToolTipText; })
			.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
			.BorderBackgroundColor(bIsHeaderTableRow
				                       ? FEditorStyle::GetColor("AnimTimeline.Outliner.HeaderColor")
				                       : FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .VAlign(VAlign_Center)
		  .AutoWidth()
		  .Padding(4.0f, 1.0f)
		[
			SNew(SExpanderArrow, InTableRow)
		]

		+ SHorizontalBox::Slot()
		  .VAlign(VAlign_Center)
		  .HAlign(HAlign_Left)
		  .Padding(2.0f, 1.0f)
		  .FillWidth(1.0f)
		[
			SNew(STextBlock)
				.TextStyle(&FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("AnimTimeline.Outliner.Label"))
				.Text_Lambda([=] { return DisplayName; })
				.HighlightText_Lambda([]
			                {
				                auto DB = GetDataBinding(FText, "TreeViewFilterText");
				                if (DB)
				                {
					                return DB->GetData();
				                }
				                return FText::GetEmpty();
			                })
		]
	];
}


#undef LOCTEXT_NAMESPACE
