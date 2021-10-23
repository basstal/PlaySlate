#pragma once

class FActEventTimeline;

class FActActionTrackEditorBase : public TSharedFromThis<FActActionTrackEditorBase>
{
public:
	FActActionTrackEditorBase(const TSharedRef<FActEventTimeline>& InSequenceController);

	virtual ~FActActionTrackEditorBase();

	/**
	 * 对于每个TrackEditor，用来构建自身在AddTrack菜单中的内容
	 * 
	 * @param MenuBuilder 被修改的菜单构建者
	 */
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) = 0;

protected:
	TWeakPtr<FActEventTimeline> ActActionSequenceController;
};
