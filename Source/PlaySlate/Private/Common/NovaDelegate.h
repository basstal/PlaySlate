#pragma once

#include "Common/NovaEnum.h"

struct FActActionHitBoxData;
class FActActionTrackEditorBase;
class SActImageTreeViewTableRow;
class SActActionSequenceTreeViewRow;
// class SActNotifyPoolNotifyNodeWidget;
class SActEventTimelineWidget;

namespace NovaDelegate
{
	using namespace NovaEnum;
	// Called to get an object (used by the asset details panel)
	DECLARE_DELEGATE_RetVal(UObject*, OnGetAssetDelegate);
	DECLARE_DELEGATE_RetVal(float, OnGetDraggedNodePosDelegate);

	/** A delegate which will create an auto-key handler. */
	DECLARE_DELEGATE_RetVal_TwoParams(FFrameNumber, OnGetNearestKeyDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_RetVal_ThreeParams(TSharedRef<SWidget>,
	                                    OnGenerateWidgetForColumnDelegate,
	                                    const TSharedRef<SActImageTreeViewTableRow>&,
	                                    const FName&,
	                                    const TSharedRef<SActActionSequenceTreeViewRow>&);
	DECLARE_DELEGATE_RetVal_ThreeParams(bool,
	                                    OnSnapPositionDelegate,
	                                    float& /*InOutTimeToSnap*/,
	                                    float /*InSnapMargin*/,
	                                    TArrayView<const FName> /*InSkippedSnapTypes*/)
	/** Called back when a details panel is created */
	DECLARE_DELEGATE_OneParam(OnDetailsCreatedDelegate, const TSharedRef<class IDetailsView>&);
	DECLARE_DELEGATE_OneParam(OnTimeRangeChangedDelegate, TRange<float>)
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
	/** A delegate that is executed when adding menu content. */
	DECLARE_DELEGATE_TwoParams(OnSetMarkedFrameDelegate, int32, FFrameNumber)
	DECLARE_DELEGATE_TwoParams(OnPanTrackRequestDelegate, int32, FVector2D)

	DECLARE_MULTICAST_DELEGATE_OneParam(OnAssetPropertiesModifiedMulticastDelegate, UObject*);
}
