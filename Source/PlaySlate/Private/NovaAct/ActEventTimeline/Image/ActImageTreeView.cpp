#include "ActImageTreeView.h"

#include "ActImagePoolWidget.h"
#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/ActImagePoolAreaPanel.h"

#include "Common/NovaDataBinding.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "NovaAct/Assets/ActAnimation.h"


SActImageTreeView::~SActImageTreeView()
{
	NovaDB::Delete("ActImageTrack/Refresh");
	auto DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	if (DB)
	{
		DB->UnBind(OnHitBoxesChangedHandle);
	}
}

void SActImageTreeView::Construct(const FArguments& InArgs, const TSharedRef<SActImagePoolAreaPanel>& InActImageTrackAreaPanel)
{
	NovaDB::CreateSP<IActImageTrackBase>("ActImageTrack/Refresh", nullptr);
	ActImageAreaPanel = InActImageTrackAreaPanel;

	TextFilter = MakeShareable(new FTextFilterExpressionEvaluator(ETextFilterExpressionEvaluatorMode::BasicString));

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed)
		+ SHeaderRow::Column(FName("Outliner"))
		.FillWidth(1.0f);
	FArguments TreeViewArgs;
	{
		TreeViewArgs._TreeItemsSource = InArgs._TreeItemsSource;
		TreeViewArgs._SelectionMode = ESelectionMode::Single;
		TreeViewArgs._HeaderRow = HeaderRow;
		TreeViewArgs._HighlightParentNodesForSelection = true;
		TreeViewArgs._AllowInvisibleItemSelection = true;
		TreeViewArgs._Clipping = EWidgetClipping::ClipToBounds;
		TreeViewArgs._OnGetChildren.BindRaw(this, &SActImageTreeView::OnGetChildren);
		TreeViewArgs._OnExpansionChanged.BindRaw(this, &SActImageTreeView::OnExpansionChanged);
		TreeViewArgs._ExternalScrollbar = InArgs._ExternalScrollbar;
		TreeViewArgs._OnGenerateRow.BindRaw(this, &SActImageTreeView::OnTreeViewGenerateRow);
	}
	STreeView::Construct(TreeViewArgs);

	DataBindingUObjectBindRaw(UActAnimation, "ActAnimation", this, &SActImageTreeView::OnHitBoxesChanged, OnHitBoxesChangedHandle);
	if (!NovaDB::Get("TreeViewFilterText"))
	{
		NovaDB::Create("TreeViewFilterText", FText::GetEmpty());
	}
	DataBindingBindRaw(FText, "TreeViewFilterText", this, &SActImageTreeView::OnFilterChanged, OnFilterChangedHandle);
}

void SActImageTreeView::OnGetChildren(TSharedRef<SActImageTreeViewTableRow> InParent,
                                      TArray<TSharedRef<SActImageTreeViewTableRow>>& OutChildren) const
{
	for (const auto& Node : InParent->GetChildNodes())
	{
		if (!Node->IsHidden())
		{
			OutChildren.Add(Node);
		}
	}
}

void SActImageTreeView::OnExpansionChanged(TSharedRef<SActImageTreeViewTableRow> InDisplayNode, bool bIsExpanded)
{
	// UE_LOG(LogNovaAct, Log, TEXT("InDisplayNode->GetPathName : %s, bIsExpanded : %d"), *InDisplayNode->GetPathName(), bIsExpanded);
	typedef TMap<TSharedPtr<IActImageTrackBase>, TWeakPtr<SActImagePoolWidget>> PoolWidgetMap;
	auto DB = GetDataBindingSP(PoolWidgetMap, "ImageTrack2LaneWidget");
	if (!DB)
	{
		return;
	}

	if (TSharedPtr<PoolWidgetMap> ImageTrack2LaneWidget = DB->GetData())
	{
		for (const TSharedRef<SActImageTreeViewTableRow>& ChildNode : InDisplayNode->GetChildNodes())
		{
			TSharedRef<IActImageTrackBase> ActImageTrack = ChildNode->GetActImageTrack();
			TWeakPtr<SActImagePoolWidget>* WeakTrackPanelPtr = ImageTrack2LaneWidget->Find(ActImageTrack);
			if (WeakTrackPanelPtr && WeakTrackPanelPtr->IsValid())
			{
				ImageTrack2LaneWidget->FindChecked(ActImageTrack).Pin()->SetVisibility(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
			}
		}
	}
}


void SActImageTreeView::Refresh()
{
	// DisplayedRootNodes.Reset();
	// for (auto& Item : ChildNodes)
	// {
	// 	if (Item->IsVisible())
	// 	{
	// 		DisplayedRootNodes.Add(Item);
	// 	}
	// }
	// SetTreeItemsSource(&DisplayedRootNodes);
}


void SActImageTreeView::OnHitBoxesChanged(UActAnimation* InActAnimation)
{
	//
	// int32 HitBoxTreeViewNodeCount = HitBoxesFolder->GetChildNodes().Num();
	// if (HitBoxTreeViewNodeCount < InHitBoxData.Num())
	// {
	// 	FName HitBoxName("HitBox");
	// 	for (int32 count = HitBoxTreeViewNodeCount; count < InHitBoxData.Num(); ++count)
	// 	{
	// 		// DisplayedRootNodes.Add(NewTreeViewNode);
	// 	}
	// }
	// int32 Index = 0;
	// for (FActActionHitBoxData& InHitBox : InHitBoxData)
	// {
	// 	HitBoxesFolder->GetChildByIndex(Index++)->SetContentAsHitBox(InHitBox);
	// }
	// int32 ChildCount = HitBoxesFolder->GetChildNodes().Num();
	// while (Index < ChildCount)
	// {
	// 	HitBoxesFolder->GetChildByIndex(Index++)->RemoveFromParent();
	// }
	// SetTreeItemsSource(&DisplayedRootNodes);
	// RequestListRefresh();
}


TSharedRef<ITableRow> SActImageTreeView::OnTreeViewGenerateRow(TSharedRef<SActImageTreeViewTableRow> InActImageTreeViewTableRow,
                                                               const TSharedRef<STableViewBase>& OwnerTable)
{
	typedef TMap<TSharedPtr<IActImageTrackBase>, TWeakPtr<SActImagePoolWidget>> PoolWidgetMap;
	auto DB = GetDataBindingSP(PoolWidgetMap, "ImageTrack2LaneWidget");
	if (!DB)
	{
		UE_LOG(LogNovaAct, Error, TEXT("DataBindingSP for type TMap<TSharedPtr<IActImageTrackBase>, TWeakPtr<SActImagePoolWidget>> don't exist"));
		return InActImageTreeViewTableRow;
	}
	TSharedPtr<PoolWidgetMap> ImageTrack2LaneWidget = DB->GetData();
	if (!ImageTrack2LaneWidget)
	{
		UE_LOG(LogNovaAct, Error, TEXT("ImageTrack2LaneWidget is nullptr"));
		return InActImageTreeViewTableRow;
	}
	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	TSharedRef<IActImageTrackBase> ActImageTrack = InActImageTreeViewTableRow->GetActImageTrack();
	TSharedPtr<SActImagePoolWidget> LaneWidget = ImageTrack2LaneWidget->FindRef(ActImageTrack).Pin();

	if (!LaneWidget)
	{
		// Add a track slot for the row
		LaneWidget = ActImageAreaPanel->MakeLaneWidgetForTrack(ActImageTrack);
		ImageTrack2LaneWidget->Add(ActImageTrack, LaneWidget);
	}

	return InActImageTreeViewTableRow;
}


void SActImageTreeView::OnFilterChanged(FText InFilterText)
{
	if (InFilterText.IsEmpty())
	{
		return;
	}
	TextFilter->SetFilterText(InFilterText);
	RequestTreeRefresh();
}
