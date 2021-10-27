#pragma once

#include "Common/NovaStruct.h"

class UActAnimation;
class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActImageTreeView;
class SActImageTrackAreaPanel;
class SActImageTrackLaneWidget;
class SActImageTrackCarWidget;
class SActTrackPanel;

using namespace NovaEnum;
using namespace NovaStruct;
/**
 * 控制整个树逻辑
 */
class SActImageHorizontalBox : public SHorizontalBox
{
public:
	SLATE_BEGIN_ARGS(SActImageHorizontalBox) {}
	SLATE_END_ARGS()

	virtual ~SActImageHorizontalBox() override;
	
	void Construct(const FArguments& InArgs);

	/**
	 * 依据特定的数据生成一行 TableRow，供TreeView回调使用，同时在TrackAreaPanel也生成一个对应的TrackLaneWidget 
	 * @param InTreeViewNode
	 * @param OwnerTable
	 * @return 
	 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedRef<SActImageTreeViewTableRow> InTreeViewNode, const TSharedRef<STableViewBase>& OwnerTable);

protected:
	TSharedPtr<SActImageTrackAreaPanel> ActImageTrackAreaPanel;// ** Track 区域 Panel 面板，用于构建 TrackLaneWidget
	TSharedPtr<SActImageTreeView> ActImageTreeView;
	TSharedPtr<SActImageTrackAreaPanel> PinnedActImageTrackAreaPanel;// ** Track 区域 Panel 面板，用于构建 TrackLaneWidget
	TSharedPtr<SActImageTreeView> PinnedActImageTreeView;

	TMap<TSharedPtr<SActImageTreeViewTableRow>, TWeakPtr<SActImageTrackLaneWidget>> TreeViewNode2TrackLane;// ** A map of child slot content that exist in our view.
};
