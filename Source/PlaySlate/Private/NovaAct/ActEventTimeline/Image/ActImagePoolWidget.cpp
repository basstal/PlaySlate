#include "ActImagePoolWidget.h"

#include "ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/PoolWidgetTypes/ActPoolWidgetNotifyWidget.h"
#include "NovaAct/ActEventTimeline/Image/PoolWidgetTypes/ActPoolWidgetFolderWidget.h"
#include "Widgets/SWeakWidget.h"

SActImagePoolWidget::Slot::Slot(const TSharedRef<SActImagePoolWidget>& InSlotContent)
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

float SActImagePoolWidget::Slot::GetVerticalOffset() const
{
	return SlotContent.IsValid() ? SlotContent->GetPhysicalPosition() : 0.f;
}

void SActImagePoolWidget::Construct(const FArguments& InArgs, const TSharedRef<IActImageTrackBase>& InActImageTack)
{
	ActImageTrack = InActImageTack;
	ChildSlot
	[
		InActImageTack->GenerateContentWidgetForLaneWidget(SharedThis(this))
	];
}

// void SActImagePoolWidget::Update()
// {
// 	// ** TODO:改成数据绑定就不需要类型转换了
// 	EActImageTrackType Type = ActImageTreeViewTableRow->GetTableRowType();
// 	switch (Type)
// 	{
// 	case EActImageTrackType::None: break;
// 	case EActImageTrackType::Folder: break;
// 	case EActImageTrackType::Notify:
// 		{
// 			StaticCastSharedPtr<SActPoolWidgetNotifyWidget>(ChildPanel)->Update();
// 			break;
// 		}
// 	default: ;
// 	}
// }

float SActImagePoolWidget::GetPhysicalPosition() const
{
	if (ActImageTrack.IsValid())
	{
		typedef TMap<TSharedPtr<IActImageTrackBase>, TWeakPtr<SActImageTreeViewTableRow>> TableRowMap;
		auto DB = GetDataBindingSP(TableRowMap, "ImageTrack2TableRow");
		if (DB)
		{
			TSharedPtr<TableRowMap> ImageTrack2TableRow = DB->GetData();
			if (auto* WeakTableRowPtr = ImageTrack2TableRow->Find(ActImageTrack))
			{
				return WeakTableRowPtr->Pin()->ComputeTrackPosition();
			}
		}
	}
	return 0.0f;
}
