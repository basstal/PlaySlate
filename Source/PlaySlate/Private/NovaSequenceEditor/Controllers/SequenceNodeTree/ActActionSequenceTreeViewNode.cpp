#include "ActActionSequenceTreeViewNode.h"

#include "ActActionSequenceSectionBase.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceCombinedKeysTrack.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceSectionArea.h"

FActActionSequenceTreeViewNode::FActActionSequenceTreeViewNode(const TSharedRef<FActActionSequenceController>& InActActionSequenceController, const TSharedPtr<FActActionSequenceTreeViewNode>& InParentTree, FName InNodeName)
	: NodeName(InNodeName),
	  ParentNode(InParentTree),
	  ActActionSequenceController(InActActionSequenceController)
{
}

FActActionSequenceTreeViewNode::~FActActionSequenceTreeViewNode()
{
}

bool FActActionSequenceTreeViewNode::IsTreeViewRoot()
{
	return ParentNode == nullptr;
}

const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& FActActionSequenceTreeViewNode::GetChildNodes() const
{
	return ChildNodes;
}


void FActActionSequenceTreeViewNode::SetParent(TSharedPtr<FActActionSequenceTreeViewNode> InParent, int32 DesiredChildIndex)
{
	TSharedPtr<FActActionSequenceTreeViewNode> CurrentParent = ParentNode;
	if (CurrentParent != InParent)
	{
		const FString OldPath = GetPathName();

		TSharedRef<FActActionSequenceTreeViewNode> ThisNode = AsShared();
		if (CurrentParent)
		{
			// Remove from parent
			CurrentParent->ChildNodes.Remove(ThisNode);
		}

		if (InParent)
		{
			// Add to new parent
			if (DesiredChildIndex != INDEX_NONE && ensureMsgf(DesiredChildIndex <= InParent->ChildNodes.Num(), TEXT("Invalid insert index specified")))
			{
				InParent->ChildNodes.Insert(ThisNode, DesiredChildIndex);
			}
			else
			{
				InParent->ChildNodes.Add(ThisNode);
			}

			// bExpanded = ParentNode.GetSavedExpansionState(*this);

			// if (InParent != ParentNode.GetRootNode())
			// {
			// 	bPinned = false;
			// 	ParentNode.SavePinnedState(*this, false);
			// }
		}

		ParentNode = InParent;

		// ParentNode->GetSequence().OnNodePathChanged(OldPath, GetPathName());
	}
}


FString FActActionSequenceTreeViewNode::GetPathName() const
{
	// First get our parent's path
	FString PathName;

	TSharedPtr<FActActionSequenceTreeViewNode> Parent = GetParent();
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
	// return ParentNode.HasActiveFilter() && !ParentNode.IsNodeFiltered(AsShared());
	return false;
}

TSharedRef<SWidget> FActActionSequenceTreeViewNode::GenerateContainerWidgetForOutliner(const TSharedRef<SActActionSequenceTreeViewRow>& InRow)
{
	TSharedPtr<SWidget> NewWidget = SNew(SActActionOutlinerTreeNode, SharedThis(this), InRow);
	// .IconBrush(this, &FActActionSequenceTreeViewNode::GetIconBrush)
	// .IconColor(this, &FActActionSequenceTreeViewNode::GetIconColor)
	// .IconOverlayBrush(this, &FActActionSequenceTreeViewNode::GetIconOverlayBrush)
	// .IconToolTipText(this, &FActActionSequenceTreeViewNode::GetIconToolTipText)
	// .CustomContent()
	// [
	// 	GetCustomOutlinerContent()
	// ];

	return NewWidget.ToSharedRef();
}


ActActionSequence::ESequenceNodeType FActActionSequenceTreeViewNode::GetType()
{
	return ActActionSequence::ESequenceNodeType::Root;
}

bool FActActionSequenceTreeViewNode::IsSelectable() const
{
	return true;
}

TSharedPtr<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::GetParent() const
{
	TSharedPtr<FActActionSequenceTreeViewNode> Pinned = ParentNode;
	return (Pinned && Pinned->GetType() != ActActionSequence::ESequenceNodeType::Root) ? Pinned : nullptr;
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
			Authority = Authority->GetParent();
		}
	}

	return Authority;
}

bool FActActionSequenceTreeViewNode::IsVisible() const
{
	return true;
}

TSharedRef<SWidget> FActActionSequenceTreeViewNode::GenerateWidgetForSectionArea(const TAttribute<TRange<double>>& ViewRange)
{
	if (GetType() == ActActionSequence::ESequenceNodeType::Track)
	{
		return SNew(SActActionSequenceSectionArea, SharedThis(this))
			.ViewRange(ViewRange);
	}

	auto VisibilityLambda = [this]()
	{
		return EVisibility::Visible;
	};
	return SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
		.ViewRange(ViewRange)
		.Visibility_Lambda(VisibilityLambda)
		.TickResolution(this, &FActActionSequenceTreeViewNode::GetTickResolution);
}

TArray<TSharedRef<FActActionSequenceSectionBase>>& FActActionSequenceTreeViewNode::GetSections()
{
	return Sections;
}

float FActActionSequenceTreeViewNode::GetNodeHeight() const
{
	float SectionHeight = Sections.Num() > 0
		                      ? Sections[0]->GetSectionHeight()
		                      : 15.0f;
	float PaddedSectionHeight = SectionHeight + 6.0f;

	return PaddedSectionHeight;
}

TSharedRef<FActActionSequenceController> FActActionSequenceTreeViewNode::GetSequence() const
{
	return ActActionSequenceController.Pin().ToSharedRef();
}

FFrameRate FActActionSequenceTreeViewNode::GetTickResolution() const
{
	return GetSequence()->GetActActionSequenceEditor()->GetTickResolution();
}

TSharedPtr<FActActionSequenceTreeViewNode> FActActionSequenceTreeViewNode::GetParentOrRoot() const
{
	return ParentNode;
}

bool FActActionSequenceTreeViewNode::CanRenameNode() const
{
	return true;
}

FSlateFontInfo FActActionSequenceTreeViewNode::GetDisplayNameFont() const
{
	FSlateFontInfo NodeFont = FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");
	return NodeFont;
}

FSlateColor FActActionSequenceTreeViewNode::GetDisplayNameColor() const
{
	return FLinearColor(0.6f, 0.6f, 0.6f, 0.6f);
}

bool FActActionSequenceTreeViewNode::ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const
{
	if (NewDisplayName.IsEmpty())
	{
		OutErrorMessage = NSLOCTEXT("Sequencer", "RenameFailed_LeftBlank", "Labels cannot be left blank");
		return false;
	}
	else if (NewDisplayName.ToString().Len() >= NAME_SIZE)
	{
		OutErrorMessage = FText::Format(NSLOCTEXT("Sequencer", "RenameFailed_TooLong", "Names must be less than {0} characters long"), NAME_SIZE);
		return false;
	}
	return true;
}

void FActActionSequenceTreeViewNode::SetDisplayName(const FText& NewDisplayName)
{
	NodeName = FName(NewDisplayName.ToString());
}
