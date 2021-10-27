#include "ActImageTreeView.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"

#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActActionSequenceTreeViewRow.h"


void SActImageTreeView::Construct(const FArguments& InArgs)
{
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(this, &SActImageTreeView::OnHitBoxesChanged));

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed)
		+ SHeaderRow::Column(FName("Outliner"))
		.FillWidth(1.0f);
	FArguments TreeViewArgs;
	{
		TreeViewArgs._TreeItemsSource = &DisplayedRootNodes;
		TreeViewArgs._SelectionMode = ESelectionMode::Single;
		TreeViewArgs._HeaderRow = HeaderRow;
		TreeViewArgs._HighlightParentNodesForSelection = true;
		TreeViewArgs._AllowInvisibleItemSelection = true;
		TreeViewArgs._Clipping = EWidgetClipping::ClipToBounds;
		TreeViewArgs._OnGetChildren.BindRaw(this, &SActImageTreeView::OnGetChildren);
		TreeViewArgs._OnExpansionChanged.BindRaw(this, &SActImageTreeView::OnExpansionChanged);
		TreeViewArgs._ExternalScrollbar = InArgs._ExternalScrollbar;
		TreeViewArgs._OnGenerateRow = InArgs._OnGenerateRow;
	}
	STreeView::Construct(TreeViewArgs);
}

void SActImageTreeView::OnGetChildren(TSharedRef<SActImageTreeViewTableRow> InParent, TArray<TSharedRef<SActImageTreeViewTableRow>>& OutChildren) const
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
	UE_LOG(LogNovaAct, Log, TEXT("InDisplayNode->GetPathName : %s, bIsExpanded : %d"), *InDisplayNode->GetPathName(), bIsExpanded);
	for (const TSharedRef<SActImageTreeViewTableRow>& ChildNode : InDisplayNode->GetChildNodes())
	{
		ChildNode->SetVisible(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
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
	TArray<FActActionHitBoxData>& InHitBoxData = InActAnimation->ActActionHitBoxes;
	FName HitBoxesFolderName("HitBoxesFolder");
	auto FindElement = DisplayedRootNodes.FindByPredicate([HitBoxesFolderName](TSharedRef<SActImageTreeViewTableRow> InNode)
	{
		return InNode->GetNodeName() == HitBoxesFolderName;
	});
	TSharedPtr<SActImageTreeViewTableRow> HitBoxesFolder;
	if (!FindElement)
	{
		HitBoxesFolder = SNew(SActImageTreeViewTableRow, SharedThis(this), HitBoxesFolderName, ENovaTreeViewNodeType::Folder);
		DisplayedRootNodes.Add(HitBoxesFolder.ToSharedRef());
	}
	else
	{
		HitBoxesFolder = *FindElement;
	}
	int HitBoxTreeViewNodeCount = HitBoxesFolder->GetChildNodes().Num();
	if (HitBoxTreeViewNodeCount < InHitBoxData.Num())
	{
		FName HitBoxName("HitBox");
		for (int count = HitBoxTreeViewNodeCount; count < InHitBoxData.Num(); ++count)
		{
			TSharedRef<SActImageTreeViewTableRow> NewTreeViewNode = SNew(SActImageTreeViewTableRow, SharedThis(this), HitBoxName, ENovaTreeViewNodeType::State);
			NewTreeViewNode->SetParent(HitBoxesFolder);
		}
	}
	int Index = 0;
	for (FActActionHitBoxData& InHitBox : InHitBoxData)
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetContentAsHitBox(InHitBox);
	}
	while (Index < HitBoxesFolder->GetChildNodes().Num())
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetVisible(EVisibility::Collapsed);
	}
	Refresh();
}
