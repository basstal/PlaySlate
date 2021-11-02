#include "ActImageTrackPanel.h"

#include "ActImageTreeViewTableRow.h"
#include "Subs/ActTrackPanelNotifiesPanel.h"
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
	case ENovaTreeViewTableRowType::Folder: break;
	case ENovaTreeViewTableRowType::Notifies:
		{
			ChildSlot
			[
				SAssignNew(ChildPanel, SActTrackPanelNotifiesPanel)
			];
			break;
		}
	default: ;
	}
}

void SActImageTrackPanel::Update()
{
	// ** TODO:改成数据绑定就不需要类型转换了
	ENovaTreeViewTableRowType Type = ActImageTreeViewTableRow->GetTableRowType();
	switch (Type)
	{
	case ENovaTreeViewTableRowType::None: break;
	case ENovaTreeViewTableRowType::Folder: break;
	case ENovaTreeViewTableRowType::Notifies:
		{
			StaticCastSharedPtr<SActTrackPanelNotifiesPanel>(ChildPanel)->Update();
			break;
		}
	default: ;
	}
}

float SActImageTrackPanel::GetPhysicalPosition() const
{
	// TODO:
	// TSharedPtr<FAnimTimelineTrack> Track = WeakTrack.Pin();
	// if (Track.IsValid())
	// {
	// return ActImageTreeViewTableRow->ComputeTrackPosition(Track.ToSharedRef()).Get(0.0f);
	// }
	return 0.0f;
}
