#include "ActImageTrackFolder.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/PoolWidgetTypes/ActPoolWidgetFolderWidget.h"

#define LOCTEXT_NAMESPACE "NovaAct"

using namespace NovaConst;

FActImageTrackFolder::FActImageTrackFolder()
{
	ActImageTrackArgs = MakeShareable(new FActImageTrackArgs());
	{
		ActImageTrackArgs->DisplayName = LOCTEXT("NovaAct_TreeViewTableRowFolderDisplayName",
		                                         "Tree view table row folder display name.");
		ActImageTrackArgs->ToolTipText = LOCTEXT("NovaAct_TreeViewTableRowFolderToolTipText",
		                                         "Tree view table row folder tool tip text.");
		ActImageTrackArgs->bIsHeaderTableRow = false;
		ActImageTrackArgs->TrackType = EActImageTrackType::Folder;
		ActImageTrackArgs->Height = NotifyHeight;
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

TSharedRef<SWidget> FActImageTrackFolder::GenerateContentWidgetForLaneWidget(const TSharedRef<SActImagePoolWidget>& InLaneWidget)
{
	return SNew(SActPoolWidgetFolderWidget, SharedThis(this));
}


#undef LOCTEXT_NAMESPACE
