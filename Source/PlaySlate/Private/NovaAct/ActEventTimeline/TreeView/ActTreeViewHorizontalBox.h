#pragma once

#include "Common/NovaStruct.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"


class UActAnimation;
class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActActionSequenceTreeView;
class SActActionSequenceTrackArea;
class FActActionTrackAreaSlot;
class SActActionSequenceTrackLane;
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
protected:
	TSharedPtr<SActActionSequenceTreeView> TreeView;        // ** 这个Controller的Widget
	TSharedPtr<SActActionSequenceTreeView> TreeViewPinned;  // ** (Pinned)这个Controller的Widget
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;      // ** Widget TreeView 对应的TrackArea，
	TSharedPtr<SActActionSequenceTrackArea> TrackAreaPinned;// ** (Pinned)Widget TreeView 对应的TrackArea

	TSharedPtr<SActTrackPanel> ActTrackPanel;// ** 代表一行Track区域
};
