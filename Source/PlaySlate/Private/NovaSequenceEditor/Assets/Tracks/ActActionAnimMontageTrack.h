#pragma once
#include "ActActionTrackEditorBase.h"

class FActActionAnimMontageTrack : public FActActionTrackEditorBase
{
public:
	FActActionAnimMontageTrack(const TSharedRef<FActActionSequenceController>& ActActionSequenceController);

	//~Begin FActActionTrackEditorBase interface
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;

	//~End FActActionTrackEditorBase interface

	/** 静态构造一个此类的实例 */
	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController);

	/** 点击后触发的事件，选择一个待AddTrack的AnimMontage */
	void AssignAnimMontage();

	/**
	 * 资源选择的回调
	 * @param InAssetData 被选择的资源
	 */
	void OnAssetSelected(const FAssetData& InAssetData);

	/**
	 * 按下回车键的回调
	 * @param InAssetData 被选择的资源列表
	 */
	void OnAssetEnterPressed(const TArray<FAssetData>& InAssetData);

protected:
	TSharedPtr<SWindow> AnimMontageSelectionDialog;
};
