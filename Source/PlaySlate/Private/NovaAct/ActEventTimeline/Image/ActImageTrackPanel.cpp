#include "ActImageTrackPanel.h"

#include "ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/TrackPanelTypes/ActTrackPanelNotifyTrackWidget.h"
#include "NovaAct/ActEventTimeline/Image/TrackPanelTypes/ActTrackPanelFolderWidget.h"
#include "Widgets/SWeakWidget.h"

SActImageTrackPanel::Slot::Slot(const TSharedRef<SActImageTrackPanel>& InSlotContent)
{
	SlotContent = InSlotContent;

	HAlignment = HAlign_Fill;
	VAlignment = VAlign_Top;

	AttachWidget(
		SNew(SWeakWidget)
		.Clipping(EWidgetClipping::ClipToBounds)
		.PossiblyNullContent(InSlotContent)
	);
}

float SActImageTrackPanel::Slot::GetVerticalOffset() const
{
	return SlotContent.IsValid() ? SlotContent->GetPhysicalPosition() : 0.f;
}

void SActImageTrackPanel::Construct(const FArguments& InArgs, const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow)
{
	ActImageTreeViewTableRow = InActImageTreeViewTableRow;
	ENovaTreeViewTableRowType Type = InActImageTreeViewTableRow->GetTableRowType();
	switch (Type)
	{
	case ENovaTreeViewTableRowType::None: break;
	case ENovaTreeViewTableRowType::Folder:
		{
			ChildPanel = SNew(SActTrackPanelFolderWidget);
			ChildSlot
			[
				ChildPanel.ToSharedRef()
			];
			break;
		}
	case ENovaTreeViewTableRowType::Notify:
		{
			ChildPanel = SNew(SActTrackPanelNotifyTrackWidget,
			                  StaticCastSharedRef<FActImageTrackNotify>(InActImageTreeViewTableRow->GetActImageTrack()));
			ChildSlot
			[
				ChildPanel.ToSharedRef()
			];
			break;
		}
	default: ;
	}
}

// void SActImageTrackPanel::Update()
// {
// 	// ** TODO:改成数据绑定就不需要类型转换了
// 	ENovaTreeViewTableRowType Type = ActImageTreeViewTableRow->GetTableRowType();
// 	switch (Type)
// 	{
// 	case ENovaTreeViewTableRowType::None: break;
// 	case ENovaTreeViewTableRowType::Folder: break;
// 	case ENovaTreeViewTableRowType::Notify:
// 		{
// 			StaticCastSharedPtr<SActTrackPanelNotifyTrackWidget>(ChildPanel)->Update();
// 			break;
// 		}
// 	default: ;
// 	}
// }

float SActImageTrackPanel::GetPhysicalPosition() const
{
	if (ActImageTreeViewTableRow.IsValid())
	{
		return ActImageTreeViewTableRow->ComputeTrackPosition();
	}
	return 0.0f;
}
