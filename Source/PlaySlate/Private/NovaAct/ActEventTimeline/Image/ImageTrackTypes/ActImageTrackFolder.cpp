#include "ActImageTrackFolder.h"

#include "PlaySlate.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActImageTrackFolder::FActImageTrackFolder()
{
	ActImageTrackArgs = MakeShared<FActImageTrackArgs>();
	{
		ActImageTrackArgs->DisplayName = LOCTEXT("NovaAct_TreeViewTableRowFolderDisplayName",
		                                         "Tree view table row folder display name.");
		ActImageTrackArgs->ToolTipText = LOCTEXT("NovaAct_TreeViewTableRowFolderToolTipText",
		                                         "Tree view table row folder tool tip text.");
		ActImageTrackArgs->bIsHeaderTableRow = false;
	}
}


FActImageTrackFolder::~FActImageTrackFolder()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActImageTrackFolder::~FActImageTrackFolder"));
}

TSharedRef<SWidget> FActImageTrackFolder::GenerateContentWidgetForTableRow(
	const TSharedRef<SActImageTreeViewTableRow>& InTableRow)
{
	return SNew(SBorder)
			.ToolTipText_Lambda([=] { return ActImageTrackArgs->ToolTipText; })
			.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
			.BorderBackgroundColor(ActImageTrackArgs->bIsHeaderTableRow ?
				                       FEditorStyle::GetColor("AnimTimeline.Outliner.HeaderColor") :
				                       FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
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
				.Text_Lambda([=] { return ActImageTrackArgs->DisplayName; })
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
