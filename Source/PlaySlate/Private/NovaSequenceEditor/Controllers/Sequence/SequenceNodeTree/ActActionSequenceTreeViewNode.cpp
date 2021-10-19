#include "ActActionSequenceTreeViewNode.h"

#include "PlaySlate.h"
#include "ActActionSequenceSectionBase.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionSequenceCombinedKeysTrack.h"
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionSequenceTreeView.h"
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionSequenceTrackArea.h"

FActActionSequenceTreeViewNode::FActActionSequenceTreeViewNode(const TSharedRef<FActActionSequenceController>& InActActionSequenceController, FName InNodeName, ActActionSequence::ESequenceNodeType InNodeType)
	: ActActionSequenceController(InActActionSequenceController),
	  NodeName(InNodeName),
	  NodeType(InNodeType)
{
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
		GetCustomOutlinerContent()
	];
	return ActActionOutlinerTreeNode.ToSharedRef();
}


void FActActionSequenceTreeViewNode::MakeWidgetForSectionArea()
{
	ActActionSectionWidget = SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
		.Visibility(EVisibility::Visible)
		.TickResolution(ActActionSequenceController.Pin()->GetActActionSequenceEditor(), &FActActionSequenceEditor::GetTickResolution);
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

ActActionSequence::ESequenceNodeType FActActionSequenceTreeViewNode::GetType() const
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


TSharedPtr<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::GetSectionAreaAuthority() const
{
	TSharedPtr<FActActionSequenceTreeViewNode> Authority = SharedThis(const_cast<FActActionSequenceTreeViewNode*>(this));

	while (Authority.IsValid())
	{
		Authority = Authority->GetParentNode();
	}

	return Authority;
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

TSharedRef<SWidget> FActActionSequenceTreeViewNode::GetCustomOutlinerContent()
{
	return SNew(SSpacer);
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
		TSharedRef<FActActionSequenceTreeViewNode> Folder = MakeShareable(new FActActionSequenceTreeViewNode(ActActionSequenceController.Pin().ToSharedRef(), InName, ActActionSequence::ESequenceNodeType::Folder));
		Folder->SetParent(SharedThis(this), 0);
		return Folder;
	}
	return *FindNode;
}

void FActActionSequenceTreeViewNode::SetContentAsHitBox(const FActActionHitBoxData& InHitBox)
{
	if (ActActionOutlinerTreeNode.IsValid())
	{
	}
}

void FActActionSequenceTreeViewNode::SetVisible(EVisibility bVisible)
{
	if (ActActionOutlinerTreeNode.IsValid())
	{
		ActActionOutlinerTreeNode->SetVisibility(bVisible);
	}
}
