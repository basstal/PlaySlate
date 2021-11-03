#pragma once
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackBase.h"


class FActImageTrackNotify : public IActImageTrackBase
{
public:
	FActImageTrackNotify();
	
	//~Begin IActImageTrackBase interface
	virtual ~FActImageTrackNotify() override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) override;
	//~End IActImageTrackBase interface

	/**
	 * 刷新 Notify 的行内显示内容
	 *
	 * @param InActImageTrack
	 */
	void OnTreeViewContentRefresh(TSharedPtr<IActImageTrackBase> InActImageTrack);
	/**
	 * 重命名 Track 的回调
	 * @param InText
	 * @param CommitInfo
	 * @param TrackIndex 
	 */
	void OnCommitName(const FText& InText, ETextCommit::Type CommitInfo, int32 TrackIndex);
	/**
	 * 构造 Notify 的子菜单
	 * @param InTrackIndex 子菜单选择对象在整个 panel 中的位置
	 * @return 返回的菜单 Widget
	 */
	TSharedRef<SWidget> BuildSubMenu(int32 InTrackIndex);
	/**
	 * 插入 Track 的回调
	 * @param InTrackIndexToInsert
	 */
	void InsertNewTrack(int32 InTrackIndexToInsert);

	/**
	 * 进入重命名编辑器的定时触发
	 * 
	 * @param InCurrentTime
	 * @param InDeltaTime
	 * @param InInlineEditableTextBlock 当前重命名的 EditableTextBlock
	 * @return
	 */
	EActiveTimerReturnType PendingRenameTimer(double InCurrentTime, float InDeltaTime, TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock);
	/** 刷新所有数据 */
	void Update();
protected:
	/** 共享参数 */
	TSharedPtr<FActImageTrackArgs> ActImageTrackArgs;
	/** Notify Widget 所从属的 Box */
	TSharedPtr<SVerticalBox> NotifyContainerBox;
	int32 PendingRenameTrackIndex;
	FDelegateHandle OnTreeViewContentRefreshHandle;
};
