#include "ActActionSequenceTreeViewNode.h"

#include "PlaySlate.h"
#include "ActActionSequenceSectionBase.h"
#include "NovaAct/ActActionSequenceEditor.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"
#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineBrain.h"
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/ActActionSequenceCombinedKeysTrack.h"
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/ActActionSequenceTreeView.h"
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/ActActionSequenceTrackArea.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActActionSequenceTreeViewNode::FActActionSequenceTreeViewNode(const TSharedRef<FActEventTimelineBrain>& InActActionSequenceController, FName InNodeName, ENovaSequenceNodeType InNodeType)
	: ActActionSequenceController(InActActionSequenceController),
	  NodeName(InNodeName),
	  NodeType(InNodeType),
	  CachedHitBox(nullptr)
{
	if (NodeType == ENovaSequenceNodeType::Folder)
	{
		OutlinerContent = SNew(SBorder)
		.ToolTipText(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]()
		                               {
			                               return FText::Format(LOCTEXT("ActActionSequence", "ToolTip {0}"), FText::FromString(*this->NodeName.ToString()));
		                               })))
		.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
		.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Left)
			  .Padding(2.0f, 1.0f)
			  .FillWidth(1.0f)
			[
				SNew(STextBlock)
					.TextStyle(&FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("AnimTimeline.Outliner.Label"))
					.Text(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]()
				                {
					                return FText::Format(LOCTEXT("ActActionSequence", "{0}"), FText::FromString(*this->NodeName.ToString()));
				                })))
			]
		];
	}
	else
	{
		OutlinerContent == SNullWidget::NullWidget;
	}
}

FActActionSequenceTreeViewNode::~FActActionSequenceTreeViewNode()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionSequenceTreeViewNode::~FActActionSequenceTreeViewNode : %s"));
}

void FActActionSequenceTreeViewNode::MakeActActionSequenceTreeView(const TSharedRef<SScrollBar>& ScrollBar)
{
	TrackArea = SNew(SActActionSequenceTrackArea);
	TreeView = SNew(SActActionSequenceTreeView, SharedThis(this), TrackArea.ToSharedRef())
		.ExternalScrollbar(ScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds);
}

void FActActionSequenceTreeViewNode::MakeActActionSequenceTreeViewPinned(const TSharedRef<SScrollBar>& ScrollBar)
{
	TrackAreaPinned = SNew(SActActionSequenceTrackArea);
	TreeViewPinned = SNew(SActActionSequenceTreeView, SharedThis(this), TrackAreaPinned.ToSharedRef())
		.ExternalScrollbar(ScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds);
}


TSharedRef<SActActionOutlinerTreeNode> FActActionSequenceTreeViewNode::MakeOutlinerWidget(const TSharedRef<SActActionSequenceTreeViewRow>& InRow)
{
	ActActionOutlinerTreeNode = SNew(SActActionOutlinerTreeNode, SharedThis(this), InRow)
	.IconBrush(this, &FActActionSequenceTreeViewNode::GetIconBrush)
	.IconColor(this, &FActActionSequenceTreeViewNode::GetDisplayNameColor)
	.IconOverlayBrush(this, &FActActionSequenceTreeViewNode::GetIconOverlayBrush)
	.IconToolTipText(this, &FActActionSequenceTreeViewNode::GetIconToolTipText)
	.CustomContent()
	[
		OutlinerContent ? OutlinerContent.ToSharedRef() : SNullWidget::NullWidget
	];
	return ActActionOutlinerTreeNode.ToSharedRef();
}


void FActActionSequenceTreeViewNode::MakeWidgetForSectionArea()
{
	ActActionTrackAreaArgs.ViewInputMin.Bind(TAttribute<float>::FGetter::CreateLambda([this]()
	{
		return ActActionSequenceController.Pin()->GetViewRange().GetLowerBoundValue();
	}));
	ActActionTrackAreaArgs.ViewInputMax.Bind(TAttribute<float>::FGetter::CreateLambda([this]()
	{
		return ActActionSequenceController.Pin()->GetViewRange().GetUpperBoundValue();
	}));
	ActActionTrackAreaArgs.TickResolution.Bind(TAttribute<FFrameRate>::FGetter::CreateLambda([this]()
	{
		return ActActionSequenceController.Pin()->GetActActionSequenceEditor()->GetTickResolution();
	}));
	ActActionSectionWidget = SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
		.Visibility(EVisibility::Visible)
		.TickResolution(ActActionSequenceController.Pin()->GetActActionSequenceEditor(), &FActActionSequenceEditor::GetTickResolution);
	ActActionTrackAreaSlot = MakeShareable(new FActActionTrackAreaSlot(SharedThis(this)));
	ActActionTrackAreaSlot->MakeTrackLane();
}

bool FActActionSequenceTreeViewNode::IsTreeViewRoot() const
{
	return ParentNode == nullptr;
}

const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& FActActionSequenceTreeViewNode::GetChildNodes() const
{
	return ChildNodes;
}

TSharedPtr<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::GetChildByIndex(int Index) const
{
	if (ChildNodes.Num() > Index)
	{
		return ChildNodes[Index];
	}
	return nullptr;
}

FString FActActionSequenceTreeViewNode::GetPathName() const
{
	// First get our parent's path
	FString PathName;

	const TSharedPtr<FActActionSequenceTreeViewNode> Parent = GetParentNode();
	if (Parent.IsValid())
	{
		ensure(Parent != SharedThis(this));
		PathName = Parent->GetPathName() + TEXT(".");
	}

	//then append our path
	PathName += NodeName.ToString();

	return PathName;
}

bool FActActionSequenceTreeViewNode::IsHidden() const
{
	return false;
}

bool FActActionSequenceTreeViewNode::IsSelectable() const
{
	return true;
}

bool FActActionSequenceTreeViewNode::IsVisible() const
{
	return true;
}

ENovaSequenceNodeType FActActionSequenceTreeViewNode::GetType() const
{
	return NodeType;
}

void FActActionSequenceTreeViewNode::SetParent(TSharedPtr<FActActionSequenceTreeViewNode> InParent, int32 DesiredChildIndex)
{
	if (!InParent || ParentNode == InParent)
	{
		return;
	}
	if (ParentNode)
	{
		// Remove from parent
		ParentNode->ChildNodes.Remove(SharedThis(this));
	}
	// Add to new parent
	if (DesiredChildIndex != INDEX_NONE && ensureMsgf(DesiredChildIndex <= InParent->ChildNodes.Num(), TEXT("Invalid insert index specified")))
	{
		InParent->ChildNodes.Insert(SharedThis(this), DesiredChildIndex);
	}
	else
	{
		InParent->ChildNodes.Add(SharedThis(this));
	}
	ParentNode = InParent;
}


TSharedPtr<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::GetSectionAreaAuthority()
{
	if (IsTreeViewRoot())
	{
		return nullptr;
	}
	return SharedThis(this);
}


TArray<TSharedRef<FActActionSequenceSectionBase>>& FActActionSequenceTreeViewNode::GetSections()
{
	return Sections;
}

float FActActionSequenceTreeViewNode::GetNodeHeight() const
{
	const float SectionHeight = Sections.Num() > 0 ? Sections[0]->GetSectionHeight() : 15.0f;
	const float PaddedSectionHeight = SectionHeight + 6.0f;
	return PaddedSectionHeight;
}

bool FActActionSequenceTreeViewNode::CanRenameNode() const
{
	return true;
}

FSlateFontInfo FActActionSequenceTreeViewNode::GetDisplayNameFont() const
{
	return FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");
}

FSlateColor FActActionSequenceTreeViewNode::GetDisplayNameColor() const
{
	return FLinearColor(0.6f, 0.6f, 0.6f, 0.6f);
}

bool FActActionSequenceTreeViewNode::ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const
{
	if (NewDisplayName.IsEmpty())
	{
		OutErrorMessage = NSLOCTEXT("Sequence", "RenameFailed_LeftBlank", "Labels cannot be left blank");
		return false;
	}
	else if (NewDisplayName.ToString().Len() >= NAME_SIZE)
	{
		OutErrorMessage = FText::Format(NSLOCTEXT("Sequence", "RenameFailed_TooLong", "Names must be less than {0} characters long"), NAME_SIZE);
		return false;
	}
	return true;
}

void FActActionSequenceTreeViewNode::SetDisplayName(const FText& NewDisplayName)
{
	FText OutErrorMessage;
	if (ValidateDisplayName(NewDisplayName, OutErrorMessage))
	{
		NodeName = FName(NewDisplayName.ToString());
	}
}

const FSlateBrush* FActActionSequenceTreeViewNode::GetIconBrush() const
{
	return nullptr;
}

const FSlateBrush* FActActionSequenceTreeViewNode::GetIconOverlayBrush() const
{
	return nullptr;
}

FText FActActionSequenceTreeViewNode::GetIconToolTipText() const
{
	return FText();
}

void FActActionSequenceTreeViewNode::AddDisplayNode(TSharedPtr<FActActionSequenceTreeViewNode> ChildTreeViewNode)
{
	DisplayedRootNodes.Add(ChildTreeViewNode.ToSharedRef());
	TreeView->SetTreeItemsSource(&DisplayedRootNodes);
}

void FActActionSequenceTreeViewNode::Refresh()
{
	DisplayedRootNodes.Reset();
	for (auto& Item : GetChildNodes())
	{
		if (Item->IsVisible())
		{
			DisplayedRootNodes.Add(Item);
		}
	}
	TreeView->SetTreeItemsSource(&DisplayedRootNodes);
}

TSharedRef<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::FindOrCreateFolder(const FName& InName)
{
	TSharedRef<FActActionSequenceTreeViewNode>* FindNode = ChildNodes.FindByPredicate([InName](auto ChildNode)
	{
		return ChildNode->NodeName == InName;
	});
	if (!FindNode)
	{
		TSharedRef<FActActionSequenceTreeViewNode> Folder = MakeShareable(new FActActionSequenceTreeViewNode(ActActionSequenceController.Pin().ToSharedRef(), InName, ENovaSequenceNodeType::Folder));
		Folder->SetParent(SharedThis(this), 0);
		return Folder;
	}
	return *FindNode;
}

void FActActionSequenceTreeViewNode::SetContentAsHitBox(FActActionHitBoxData& InHitBox)
{
	// ** TODO:临时先把对象存这里
	CachedHitBox = &InHitBox;
	ActActionTrackAreaArgs.Begin.Bind(TAttribute<int>::FGetter::CreateLambda([this]()
	{
		return CachedHitBox->Begin;
	}));
	ActActionTrackAreaArgs.End.Bind(TAttribute<int>::FGetter::CreateLambda([this]()
	{
		return CachedHitBox->End;
	}));
}

void FActActionSequenceTreeViewNode::SetVisible(EVisibility InVisibility)
{
	if (ActActionOutlinerTreeNode.IsValid())
	{
		ActActionOutlinerTreeNode->SetVisibility(InVisibility);
	}
	if (ActActionTrackAreaSlot.IsValid())
	{
		ActActionTrackAreaSlot->SetVisibility(InVisibility);
	}
}

float FActActionSequenceTreeViewNode::ComputeTrackPosition()
{
	// Positioning strategy:
	// Attempt to root out any visible track in the specified track's sub-hierarchy, and compute the track's offset from that
	const FGeometry& CachedGeometryOutlinerTreeNode = ActActionOutlinerTreeNode->GetCachedGeometry();
	// UE_LOG(LogActAction, Log, TEXT("CachedGeometryOutlinerTreeNode : %s"), *CachedGeometryOutlinerTreeNode.ToString());
	const FGeometry& CachedGeometryTrackArea = GetRoot()->TrackArea->GetCachedGeometry();
	return CachedGeometryOutlinerTreeNode.AbsolutePosition.Y - CachedGeometryTrackArea.AbsolutePosition.Y;
}

TSharedPtr<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::GetRoot()
{
	if (IsTreeViewRoot())
	{
		return AsShared();
	}

	TSharedPtr<FActActionSequenceTreeViewNode> RootNode = AsShared();
	while (!RootNode->IsTreeViewRoot())
	{
		RootNode = RootNode->GetParentNode();
	}
	return RootNode.ToSharedRef();
}

// EVisibility FActActionSequenceTreeViewNode::GetVisibility() const
// {
// 	if (ActActionOutlinerTreeNode.IsValid())
// 	{
// 		return ActActionOutlinerTreeNode->GetVisibility();
// 	}
// }
#undef LOCTEXT_NAMESPACE
