#pragma once

#include "ITransportControl.h"

class FActActionSequenceController;
class FActActionSequenceEditor;

class SActActionSequenceTransportControls : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SActActionSequenceTransportControls)
		{
		}

	SLATE_END_ARGS()

	virtual ~SActActionSequenceTransportControls() override;

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InActActionSequenceController);

	/** 点击逐帧前进的回调 */
	FReply OnClick_Forward_Step() const;

	/** 点击到动画结尾的回调 */
	FReply OnClick_Forward_End() const;

	/** 点击逐帧退后的回调 */
	FReply OnClick_Backward_Step() const;

	/** 点击到动画开始的回调 */
	FReply OnClick_Backward_End() const;

	/** 点击动画前进播放的回调 */
	FReply OnClick_Forward() const;

	/** 点击动画倒退播放的回调 */
	FReply OnClick_Backward() const;

	/** 点击循环的回调 */
	FReply OnClick_ToggleLoop() const;
	/** 是否在循环播放 */
	bool IsLoopStatusOn() const;
	/** 获得当前播放状态 */
	EPlaybackMode::Type GetPlaybackMode() const;
protected:
	/** 对Editor的引用，以获取控制播放的对象 */
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
};
