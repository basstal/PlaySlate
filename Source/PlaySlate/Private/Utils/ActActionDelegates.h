#pragma once

#include "ActActionBasics.h"
#include "NovaSequenceEditor/Assets/ActActionSequenceStructs.h"

class FActActionTrackEditorBase;
struct FPointerEvent;
class FActActionSequenceController;
class FActActionSequenceTreeViewNode;
class SActActionSequenceTreeViewRow;
class SActActionSequenceNotifyNode;

namespace ActActionSequence
{
	// Called to get an object (used by the asset details panel)
	DECLARE_DELEGATE_RetVal(UObject*, OnGetAssetDelegate);
	DECLARE_DELEGATE_RetVal(float, OnGetDraggedNodePosDelegate);
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FActActionTrackEditorBase>, OnCreateTrackEditorDelegate, TSharedRef<FActActionSequenceController>);
	/** A delegate which will create an auto-key handler. */
	DECLARE_DELEGATE_RetVal_TwoParams(FFrameNumber, OnGetNearestKeyDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_RetVal_ThreeParams(TSharedRef<SWidget>, OnGenerateWidgetForColumnDelegate, const TSharedRef<FActActionSequenceTreeViewNode>&, const FName&, const TSharedRef<SActActionSequenceTreeViewRow>&);
	DECLARE_DELEGATE_RetVal_ThreeParams(bool, OnSnapPositionDelegate, float& /*InOutTimeToSnap*/, float /*InSnapMargin*/, TArrayView<const FName> /*InSkippedSnapTypes*/)
	DECLARE_DELEGATE_RetVal_FourParams(FReply, OnNotifyNodeDragStartedDelegate, TSharedRef<SActActionSequenceNotifyNode>, const FPointerEvent&, const FVector2D&, const bool)

	/** Called back when a details panel is created */
	DECLARE_DELEGATE_OneParam(OnDetailsCreatedDelegate, const TSharedRef<class IDetailsView>&);
	DECLARE_DELEGATE_OneParam(OnTimeRangeChangedDelegate, TRange<double>)
	DECLARE_DELEGATE_OneParam(OnFrameRangeChangedDelegate, TRange<FFrameNumber>)
	DECLARE_DELEGATE_OneParam(OnAddMarkedFrameDelegate, FFrameNumber)
	DECLARE_DELEGATE_OneParam(OnDeleteMarkedFrameDelegate, int32)
	DECLARE_DELEGATE_OneParam(OnGetContextMenuContentDelegate, FMenuBuilder&);
	/** Called when an asset is selected in the asset view */
	DECLARE_DELEGATE_OneParam(OnAssetSelectedDelegate, const FAssetData& /*AssetData*/);
	/** Called when enter is pressed on an asset in the asset view */
	DECLARE_DELEGATE_OneParam(OnAssetEnterPressedDelegate, const TArray<FAssetData>& /*SelectedAssets*/);
	/** A delegate that is executed when menu object is clicked. Unlike FExtender delegates we pass in the FGuid which exists even for deleted objects. */
	DECLARE_DELEGATE_TwoParams(OnBuildCustomContextMenuForGuidDelegate, FMenuBuilder&, FGuid);
	DECLARE_DELEGATE_TwoParams(OnScrubPositionChangedDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_TwoParams(OnViewRangeChangedDelegate, TRange<double>, EActActionViewRangeInterpolation)
	/** A delegate that is executed when adding menu content. */
	DECLARE_DELEGATE_TwoParams(OnGetAddMenuContentDelegate, FMenuBuilder& /*MenuBuilder*/, TSharedRef<FActActionSequenceController>);
	DECLARE_DELEGATE_TwoParams(OnSetMarkedFrameDelegate, int32, FFrameNumber)
	DECLARE_DELEGATE_TwoParams(OnPanTrackRequestDelegate, int32, FVector2D)
	DECLARE_DELEGATE_FourParams(OnNotifyStateHandleBeingDraggedDelegate, TSharedPtr<SActActionSequenceNotifyNode> /*NotifyNode*/, const FPointerEvent& /*PointerEvent*/, ENotifyStateHandleHit /*Handle*/, float /*Time*/)

	DECLARE_MULTICAST_DELEGATE_OneParam(OnAssetPropertiesModifiedMulticastDelegate, UObject*);
	DECLARE_MULTICAST_DELEGATE_OneParam(OnHitBoxesChangedMulticastDelegate, const TArray<FActActionHitBoxData>&);
}
