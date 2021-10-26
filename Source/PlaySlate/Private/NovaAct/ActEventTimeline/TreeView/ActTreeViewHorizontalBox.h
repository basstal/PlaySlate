#pragma once

#include "Common/NovaStruct.h"

class UActAnimation;
class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActTreeView;
class SActTreeViewTrackAreaPanel;
class SActTreeViewTrackLaneWidget;
class SActTreeViewTrackCarWidget;
class SActTrackPanel;

using namespace NovaEnum;
using namespace NovaStruct;
/**
 * 控制整个树逻辑
 */
class SActTreeViewHorizontalBox : public SHorizontalBox
{
public:
	SLATE_BEGIN_ARGS(SActTreeViewHorizontalBox) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/**
	 * 依据特定的数据生成一行 TableRow，供TreeView回调使用，同时在TrackAreaPanel也生成一个对应的TrackLaneWidget 
	 * @param InDisplayNode
	 * @param OwnerTable
	 * @return 
	 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedRef<SActTreeViewNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable);

protected:
	TSharedPtr<SActTreeViewTrackAreaPanel> ActTreeViewTrackAreaPanel;// ** Track 区域 Panel 面板，用于构建 TrackLaneWidget

	TMap<TSharedPtr<SActTreeViewNode>, TWeakPtr<SActTreeViewTrackLaneWidget>> TreeViewNode2TrackLane;// ** A map of child slot content that exist in our view.
};
