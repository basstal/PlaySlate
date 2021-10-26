#include "ActTreeView.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewTrackAreaPanel.h"
#include "NovaAct/ActEventTimeline/TreeView/Subs/ActTreeViewTrackCarWidget.h"
#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewNode.h"

#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/ActAnimation.h"
#include "Subs/ActActionSequenceTreeViewRow.h"


void SActTreeView::Construct(FArguments& InArgs, const TSharedRef<SActTreeViewTrackAreaPanel>& InTrackArea)
{
	ActTreeViewTrackAreaPanel = InTrackArea;

	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(this, &SActTreeView::OnHitBoxesChanged));

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed)
		+ SHeaderRow::Column(FName("Outliner"))
		.FillWidth(1.0f);
	InArgs._TreeItemsSource = &DisplayedRootNodes;
	InArgs._SelectionMode = ESelectionMode::Single;
	InArgs._HeaderRow = HeaderRow;
	InArgs._HighlightParentNodesForSelection = true;
	InArgs._AllowInvisibleItemSelection = true;
	InArgs._Clipping = EWidgetClipping::ClipToBounds;
	InArgs._OnGetChildren = FOnGetChildren::CreateRaw(this, &SActTreeView::OnGetChildren);
	InArgs._OnExpansionChanged = FOnExpansionChanged::CreateRaw(this, &SActTreeView::OnExpansionChanged);

	STreeView::Construct(InArgs);
}


TSharedRef<SWidget> SActTreeView::GenerateWidgetFromColumn(const TSharedRef<SActTreeViewNode>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const
{
	if (ColumnId == "Outliner")
	{
		return InNode->MakeOutlinerWidget(Row);
	}
	return SNullWidget::NullWidget;
}

void SActTreeView::OnGetChildren(TSharedRef<SActTreeViewNode> InParent, TArray<TSharedRef<SActTreeViewNode>>& OutChildren) const
{
	for (const auto& Node : InParent->GetChildNodes())
	{
		if (!Node->IsHidden())
		{
			OutChildren.Add(Node);
		}
	}
}

void SActTreeView::OnExpansionChanged(TSharedRef<SActTreeViewNode> InDisplayNode, bool bIsExpanded)
{
	UE_LOG(LogNovaAct, Log, TEXT("InDisplayNode->GetPathName : %s, bIsExpanded : %d"), *InDisplayNode->GetPathName(), bIsExpanded);
	for (const TSharedRef<SActTreeViewNode>& ChildNode : InDisplayNode->GetChildNodes())
	{
		ChildNode->SetVisible(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
	}
}


void SActTreeView::Refresh()
{
	DisplayedRootNodes.Reset();
	for (auto& Item : ChildNodes)
	{
		if (Item.Value->IsVisible())
		{
			DisplayedRootNodes.Add(Item.Value);
		}
	}
	SetTreeItemsSource(&DisplayedRootNodes);
}


void SActTreeView::OnHitBoxesChanged(UActAnimation* InActAnimation)
{
	TArray<FActActionHitBoxData>& InHitBoxData = InActAnimation->ActActionHitBoxes;
	const TSharedRef<SActTreeViewNode> HitBoxesFolder = FindOrCreateFolder(FName("HitBoxesFolder"));
	int HitBoxTreeViewNodeCount = HitBoxesFolder->GetChildNodes().Num();
	if (HitBoxTreeViewNodeCount < InHitBoxData.Num())
	{
		for (int count = HitBoxTreeViewNodeCount; count < InHitBoxData.Num(); ++count)
		{
			TSharedRef<SActTreeViewNode> NewTreeViewNode = MakeShareable(new SActTreeViewNode("HitBox", ENovaSequenceNodeType::State));
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


TSharedRef<SActTreeViewNode> SActTreeView::FindOrCreateFolder(const FName& InName)
{
	TSharedRef<SActTreeViewNode>* FindNode = ChildNodes.Find(InName);
	if (!FindNode)
	{
		TSharedRef<SActTreeViewNode> Folder = MakeShareable(new SActTreeViewNode(InName, ENovaSequenceNodeType::Folder));
		return Folder;
	}
	return *FindNode;
}
