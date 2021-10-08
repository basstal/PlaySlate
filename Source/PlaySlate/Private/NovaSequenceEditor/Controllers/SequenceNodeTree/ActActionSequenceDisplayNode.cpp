#include "ActActionSequenceDisplayNode.h"

#include "ActActionSequenceSectionBase.h"
#include "ActActionSequenceNodeTree.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceCombinedKeysTrack.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceSectionArea.h"

FActActionSequenceDisplayNode::FActActionSequenceDisplayNode(const TSharedRef<FActActionSequenceNodeTree>& InParentTree, FName InNodeName)
	: NodeName(InNodeName)
	  , ParentTree(InParentTree)
{
}

FActActionSequenceDisplayNode::~FActActionSequenceDisplayNode()
{
}

const TArray<TSharedRef<FActActionSequenceDisplayNode>>& FActActionSequenceDisplayNode::GetChildNodes() const
{
	return ChildNodes;
}


void FActActionSequenceDisplayNode::SetParent(TSharedPtr<FActActionSequenceDisplayNode> InParent, int32 DesiredChildIndex)
{
	TSharedPtr<FActActionSequenceDisplayNode> CurrentParent = ParentNode.Pin();
	if (CurrentParent != InParent)
	{
		const FString OldPath = GetPathName();

		TSharedRef<FActActionSequenceDisplayNode> ThisNode = AsShared();
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

			// bExpanded = ParentTree.GetSavedExpansionState(*this);

			// if (InParent != ParentTree.GetRootNode())
			// {
			// 	bPinned = false;
			// 	ParentTree.SavePinnedState(*this, false);
			// }
		}

		ParentNode = InParent;

		// ParentTree->GetSequence().OnNodePathChanged(OldPath, GetPathName());
	}
}


FString FActActionSequenceDisplayNode::GetPathName() const
{
	// First get our parent's path
	FString PathName;

	TSharedPtr<FActActionSequenceDisplayNode> Parent = GetParent();
	if (Parent.IsValid())
	{
		ensure(Parent != SharedThis(this));
		PathName = Parent->GetPathName() + TEXT(".");
	}

	//then append our path
	PathName += NodeName.ToString();

	return PathName;
}

bool FActActionSequenceDisplayNode::IsHidden() const
{
	// return ParentTree.HasActiveFilter() && !ParentTree.IsNodeFiltered(AsShared());
	return false;
}

TSharedRef<SWidget> FActActionSequenceDisplayNode::GenerateContainerWidgetForOutliner(const TSharedRef<SActActionSequenceTreeViewRow>& InRow)
{
	
	TSharedPtr<SWidget> NewWidget = SNew(SActActionOutlinerTreeNode, SharedThis(this), InRow);
	// .IconBrush(this, &FActActionSequenceDisplayNode::GetIconBrush)
	// .IconColor(this, &FActActionSequenceDisplayNode::GetIconColor)
	// .IconOverlayBrush(this, &FActActionSequenceDisplayNode::GetIconOverlayBrush)
	// .IconToolTipText(this, &FActActionSequenceDisplayNode::GetIconToolTipText)
	// .CustomContent()
	// [
	// 	GetCustomOutlinerContent()
	// ];

	return NewWidget.ToSharedRef();
}


ActActionSequence::ESequenceNodeType FActActionSequenceDisplayNode::GetType()
{
	return ActActionSequence::ESequenceNodeType::Root;
}

bool FActActionSequenceDisplayNode::IsSelectable() const
{
	return true;
}

TSharedPtr<FActActionSequenceDisplayNode> FActActionSequenceDisplayNode::GetParent() const
{
	TSharedPtr<FActActionSequenceDisplayNode> Pinned = ParentNode.Pin();
	return (Pinned && Pinned->GetType() != ActActionSequence::ESequenceNodeType::Root) ? Pinned : nullptr;
}

TSharedPtr<FActActionSequenceDisplayNode> FActActionSequenceDisplayNode::GetSectionAreaAuthority() const
{
	TSharedPtr<FActActionSequenceDisplayNode> Authority = SharedThis(const_cast<FActActionSequenceDisplayNode*>(this));

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

bool FActActionSequenceDisplayNode::IsVisible() const
{
	return true;
}

TSharedRef<SWidget> FActActionSequenceDisplayNode::GenerateWidgetForSectionArea(const TAttribute<TRange<double>>& ViewRange)
{
	if (GetType() == ActActionSequence::ESequenceNodeType::Track)
	{
		return SNew(SActActionSequenceSectionArea, SharedThis(this))
			.ViewRange(ViewRange);
	}

	return SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
		.ViewRange(ViewRange)
		.IsEnabled(!GetSequence()->IsReadOnly())
		.Visibility_Lambda([this]()
	                                                                  {
		                                                                  return EVisibility::Visible;
	                                                                  })
		.TickResolution(this, &FActActionSequenceDisplayNode::GetTickResolution);
}

TArray<TSharedRef<FActActionSequenceSectionBase>>& FActActionSequenceDisplayNode::GetSections()
{
	return Sections;
}

float FActActionSequenceDisplayNode::GetNodeHeight() const
{
	float SectionHeight = Sections.Num() > 0
		                      ? Sections[0]->GetSectionHeight()
		                      : 15.0f;
	float PaddedSectionHeight = SectionHeight + 6.0f;

	return PaddedSectionHeight;
}

TSharedRef<FActActionSequenceController> FActActionSequenceDisplayNode::GetSequence() const
{
	return ParentTree->GetSequence();
}

FFrameRate FActActionSequenceDisplayNode::GetTickResolution() const
{
	return GetSequence()->GetFocusedTickResolution();
}

TSharedPtr<FActActionSequenceDisplayNode> FActActionSequenceDisplayNode::GetParentOrRoot() const
{
	return ParentNode.Pin();
}

bool FActActionSequenceDisplayNode::CanRenameNode() const
{
	return true;
}

FSlateFontInfo FActActionSequenceDisplayNode::GetDisplayNameFont() const
{
	FSlateFontInfo NodeFont = FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");
	return NodeFont;
}

FSlateColor FActActionSequenceDisplayNode::GetDisplayNameColor() const
{
	return FLinearColor(0.6f, 0.6f, 0.6f, 0.6f);
}

bool FActActionSequenceDisplayNode::ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const
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

void FActActionSequenceDisplayNode::SetDisplayName(const FText& NewDisplayName)
{
	NodeName = FName(NewDisplayName.ToString());
}
