#include "ActActionSequenceTreeViewNode.h"

#include "PlaySlate.h"
#include "ActActionSequenceSectionBase.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceCombinedKeysTrack.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceSectionArea.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTreeView.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"

FActActionSequenceTreeViewNode::FActActionSequenceTreeViewNode(const TSharedRef<FActActionSequenceController>& InActActionSequenceController, const TSharedPtr<FActActionSequenceTreeViewNode>& InParentNode, FName InNodeName)
	: ActActionSequenceController(InActActionSequenceController),
	  ParentNode(InParentNode),
	  NodeName(InNodeName)
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


void FActActionSequenceTreeViewNode::MakeContainerWidgetForOutliner(const TSharedRef<SActActionSequenceTreeViewRow>& InRow)
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
}


void FActActionSequenceTreeViewNode::MakeWidgetForSectionArea()
{
	if (GetType() == ActActionSequence::ESequenceNodeType::Track)
	{
		ActActionSectionWidget = SNew(SActActionSequenceSectionArea, SharedThis(this));
	}
	ActActionSectionWidget = SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
		.Visibility(EVisibility::Visible)
		.TickResolution(ActActionSequenceController.Pin()->GetActActionSequenceEditor(), &FActActionSequenceEditor::GetTickResolution);
}

bool FActActionSequenceTreeViewNode::IsTreeViewRoot()
{
	return ParentNode == nullptr;
}

const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& FActActionSequenceTreeViewNode::GetChildNodes() const
{
	return ChildNodes;
}

FString FActActionSequenceTreeViewNode::GetPathName() const
{
	// First get our parent's path
	FString PathName;

	TSharedPtr<FActActionSequenceTreeViewNode> Parent = GetParentNode();
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

ActActionSequence::ESequenceNodeType FActActionSequenceTreeViewNode::GetType()
{
	return ActActionSequence::ESequenceNodeType::Root;
}

void FActActionSequenceTreeViewNode::SetParent(TSharedPtr<FActActionSequenceTreeViewNode> InParent, int32 DesiredChildIndex)
{
	if (!InParent || ParentNode == InParent)
	{
		return;
	}
	// Remove from parent
	ParentNode->ChildNodes.Remove(SharedThis(this));
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
		if (Authority->GetType() == ActActionSequence::ESequenceNodeType::Object || Authority->GetType() == ActActionSequence::ESequenceNodeType::Track)
		{
			return Authority;
		}
		else
		{
			Authority = Authority->GetParentNode();
		}
	}

	return Authority;
}


TArray<TSharedRef<FActActionSequenceSectionBase>>& FActActionSequenceTreeViewNode::GetSections()
{
	return Sections;
}

float FActActionSequenceTreeViewNode::GetNodeHeight() const
{
	float SectionHeight = Sections.Num() > 0 ? Sections[0]->GetSectionHeight() : 15.0f;
	float PaddedSectionHeight = SectionHeight + 6.0f;
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
