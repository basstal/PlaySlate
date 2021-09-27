#include "ActActionSequenceNodeTree.h"

#include "Editor/ActActionSequenceController.h"

FActActionSequenceNodeTree::FActActionSequenceNodeTree(const TSharedRef<FActActionSequenceController>& InSequence)
	: bNeedUpdateFilter(false),
	  Sequence(InSequence)
{
}

FActActionSequenceNodeTree::~FActActionSequenceNodeTree()
{
}

void FActActionSequenceNodeTree::SetFilterNodes(const FString& InFilter)
{
	if (InFilter != CurrentFilter)
	{
		CurrentFilter = InFilter;
		bNeedUpdateFilter = true;
	}
}

bool FActActionSequenceNodeTree::HasActiveFilter() const
{
	bool bHas = !CurrentFilter.IsEmpty();
	// if (Sequence.IsValid())
	// {
	// 	bHas = bHas
	// 		|| Sequence->GetSequencerSettings()->GetShowSelectedNodesOnly()
	// 		|| Sequence->GetFocusedMovieSceneSequence()->GetMovieScene()->GetNodeGroups().HasAnyActiveFilter();
	// }
	return bHas;
}

const TArray<TSharedRef<FActActionSequenceDisplayNode>>& FActActionSequenceNodeTree::GetRootNodes() const
{
	return RootNode->GetChildNodes();
}
