#pragma once
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackBase.h"


class FActImageTrackNotify : public IActImageTrackBase
{
public:
	FActImageTrackNotify();

	//~Begin IActImageTrackBase interface
	virtual ~FActImageTrackNotify() override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForLaneWidget(const TSharedRef<SActImagePoolWidget>& InLaneWidget) override;
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
	 * 删除 Track 的回调
	 * @param InTrackIndexToRemove
	 */
	void RemoveTrack(int32 InTrackIndexToRemove);
	/**
	 * 进入重命名编辑器的定时触发
	 * 
	 * @param InCurrentTime
	 * @param InDeltaTime
	 * @param InInlineEditableTextBlock 当前重命名的 EditableTextBlock
	 * @return
	 */
	EActiveTimerReturnType PendingRenameTimer(double InCurrentTime, float InDeltaTime, TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock);
protected:
	/** Notify Widget 所从属的 Box */
	TSharedPtr<SVerticalBox> NotifyContainerBox;
	/** 重命名聚焦的 Track Index */
	int32 PendingRenameTrackIndex;
	// ** 数据绑定解绑使用的Handle
	FDelegateHandle OnTreeViewContentRefreshHandle;
};
