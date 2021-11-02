#include "ActImageTrackPanel.h"

#include "ActImageTreeViewTableRow.h"
#include "Subs/ActTrackPanelNotifiesPanel.h"

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
