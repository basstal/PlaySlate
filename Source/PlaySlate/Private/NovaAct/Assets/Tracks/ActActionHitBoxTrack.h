#pragma once
#include "ActActionTrackEditorBase.h"

class FActActionHitBoxTrack : public FActActionTrackEditorBase
{
public:
	FActActionHitBoxTrack(const TSharedRef<FActEventTimeline>& ActActionSequenceController);

	//~Begin FActActionTrackEditorBase interface
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;

	//~End FActActionTrackEditorBase interface

	/** 静态构造一个此类的实例 */
	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<FActEventTimeline> InSequenceController);

	/** 点击后触发的事件，将一个HitBox的Track添加到时间轴上 */
	void AssignHitBox() const;
};
