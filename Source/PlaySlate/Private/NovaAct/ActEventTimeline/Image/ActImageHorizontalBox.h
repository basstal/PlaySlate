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
protected:
	/** Track 区域 Panel 面板，用于构建 TrackLaneWidget */
	TSharedPtr<SActImageTrackAreaPanel> ActImageTrackAreaPanel;
	TSharedPtr<SActImageTreeView> ActImageTreeView;
	/** Track 区域 Panel 面板，用于构建 TrackLaneWidget */
	TSharedPtr<SActImageTrackAreaPanel> PinnedActImageTrackAreaPanel;
	TSharedPtr<SActImageTreeView> PinnedActImageTreeView;
};
