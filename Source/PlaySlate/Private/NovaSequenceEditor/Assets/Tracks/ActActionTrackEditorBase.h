#pragma once

class FActActionSequenceController;

class FActActionTrackEditorBase : public TSharedFromThis<FActActionTrackEditorBase>
{
public:
	FActActionTrackEditorBase(const TSharedRef<FActActionSequenceController>& InSequenceController);
	virtual ~FActActionTrackEditorBase();

	/**
	 * 对于每个TrackEditor，用来构建自身在AddTrack菜单中的内容
	 * 
	 * @param MenuBuilder 被修改的菜单构建者
	 */
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) = 0;

protected:
	TWeakPtr<FActActionSequenceController> SequenceController;
};
