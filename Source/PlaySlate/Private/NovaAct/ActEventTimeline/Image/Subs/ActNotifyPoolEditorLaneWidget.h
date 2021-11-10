#pragma once
#include "ActNotifyPoolLaneWidget.h"

class SActNotifyPoolEditorLaneWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActNotifyPoolEditorLaneWidget)
			: _LaneIndex(INDEX_NONE) {}

		SLATE_ARGUMENT(int32, LaneIndex)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	/** Index of Track in Sequence **/
	int32 LaneIndex;

public:
	/** Pointer to actual anim notify track */
	TSharedPtr<SActNotifyPoolLaneWidget> NotifyTrack;
};
