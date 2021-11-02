#pragma once
#include "NovaAct/ActEventTimeline/Image/TreeViewTableRowTypes/ActTreeViewTableRowBase.h"


class FActTreeViewTableRowNotifyTrack : public IActTreeViewTableRowBase
{
public:
	//~Begin IActTreeViewTableRowBase interface
	virtual ~FActTreeViewTableRowNotifyTrack() override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) override;
	//~End IActTreeViewTableRowBase interface

	/** 刷新 NotifyTrack 的行内显示内容 */
	void RefreshTableRow();
	/**
	 * 重命名 Track 的回调
	 * @param InText
	 * @param CommitInfo
	 * @param TrackIndex 
	 */
	void OnCommitName(const FText& InText, ETextCommit::Type CommitInfo, int32 TrackIndex);
	/**
	 * 构造 NotifyTrack 的子菜单
	 * @param InTrackIndex 子菜单选择对象在整个 panel 中的位置
	 * @return 返回的菜单 Widget
	 */
	TSharedRef<SWidget> BuildSubMenu(int32 InTrackIndex);
	/**
	 * 插入 Track 的回调
	 * @param InTrackIndexToInsert
	 */
	void InsertNewTrack(int32 InTrackIndexToInsert);

protected:
	/** Notify Widget 所从属的 Box */
	TSharedPtr<SVerticalBox> NotifyContainerBox;
};
