#pragma once

#include "ITransportControl.h"


class FNovaActEditor;

class SActTransportControlsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActTransportControlsWidget) { }
	SLATE_END_ARGS()

	virtual ~SActTransportControlsWidget() override;

	void Construct(const FArguments& InArgs);

	/** 点击逐帧前进的回调 */
	FReply OnClickForwardStep() const;

	/** 点击到动画结尾的回调 */
	FReply OnClickForwardEnd() const;

	/** 点击逐帧退后的回调 */
	FReply OnClickBackwardStep() const;

	/** 点击到动画开始的回调 */
	FReply OnClickBackwardEnd() const;

	/** 点击动画前进播放的回调 */
	FReply OnClickForwardPlay() const;

	/** 点击动画倒退播放的回调 */
	FReply OnClickBackwardPlay() const;

	/** 点击循环的回调 */
	FReply OnClickToggleLooping() const;

	/** 是否在循环播放 */
	bool IsLoopStatusOn() const;

	/** 获得当前播放状态 */
	EPlaybackMode::Type GetPlaybackMode() const;
};
