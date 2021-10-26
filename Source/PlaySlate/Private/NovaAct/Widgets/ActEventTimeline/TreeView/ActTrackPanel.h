#pragma once
#include "Common/NovaDelegate.h"

using namespace NovaDelegate;

class SActTrackPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActTrackPanel)
			: _Sequence(), _CurrentPosition(), _ViewInputMin(), _ViewInputMax(), _InputMin(), _InputMax()
		// , _OnSetInputViewRange()
		// , _OnSelectionChanged()
		// , _OnGetScrubValue()
		// , _OnRequestRefreshOffsets()
		{}

		SLATE_ARGUMENT(class UAnimSequenceBase*, Sequence)
		SLATE_ARGUMENT(float, WidgetWidth)
		SLATE_ATTRIBUTE(float, CurrentPosition)
		SLATE_ATTRIBUTE(float, ViewInputMin)
		SLATE_ATTRIBUTE(float, ViewInputMax)
		SLATE_ATTRIBUTE(float, InputMin)
		SLATE_ATTRIBUTE(float, InputMax)
		// SLATE_EVENT( OnSetInputViewRange, OnSetInputViewRange )
		// SLATE_EVENT( OnSelectionChanged, OnSelectionChanged )
		// SLATE_EVENT( OnGetScrubValue, OnGetScrubValue )
		// SLATE_EVENT( RefreshOffsetsRequest, OnRequestRefreshOffsets )
		// SLATE_EVENT( OnGetTimingNodeVisibility, OnGetTimingNodeVisibility )
		// SLATE_EVENT( OnInvokeTab, OnInvokeTab )
		SLATE_EVENT(FSimpleDelegate, OnNotifiesChanged)
		// SLATE_EVENT( OnSnapPosition, OnSnapPosition )
		// SLATE_EVENT( OnNotifyStateHandleBeingDragged, OnNotifyStateHandleBeingDragged)
		// SLATE_EVENT( OnNotifyNodesBeingDragged, OnNotifyNodesBeingDragged)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};
