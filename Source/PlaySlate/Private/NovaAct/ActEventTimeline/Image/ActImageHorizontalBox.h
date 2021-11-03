#pragma once

#include "Common/NovaStruct.h"

class UActAnimation;
class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActImageTreeView;
class SActImagePoolAreaPanel;
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
protected:
	/** TreeView Item Source */
	TArray<TSharedRef<SActImageTreeViewTableRow>> TreeViewItemSource;
	
	/** Track 区域 Panel 面板，用于构建 TrackLaneWidget */
	TSharedPtr<SActImagePoolAreaPanel> ActImageTrackAreaPanel;
	TSharedPtr<SActImageTreeView> ActImageTreeView;
	/** Track 区域 Panel 面板，用于构建 TrackLaneWidget */
	TSharedPtr<SActImagePoolAreaPanel> PinnedActImageTrackAreaPanel;
	TSharedPtr<SActImageTreeView> PinnedActImageTreeView;
};
