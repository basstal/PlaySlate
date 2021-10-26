#pragma once
#include "ActActionTrackEditorBase.h"

class SActEventTimelineWidget;

class FActActionHitBoxTrack : public FActActionTrackEditorBase
{
public:
	FActActionHitBoxTrack(const TSharedRef<SActEventTimelineWidget>& InActEventTimelineWidget);

	//~Begin FActActionTrackEditorBase interface
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;

	//~End FActActionTrackEditorBase interface

	/** 静态构造一个此类的实例 */
	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<SActEventTimelineWidget> InActEventTimelineWidget);

	/** 点击后触发的事件，将一个HitBox的Track添加到时间轴上 */
	void AssignHitBox() const;
};
