#pragma once
#include "ActActionBasics.h"

class FActActionTrackEditorBase;
class FActActionSequenceController;

namespace ActActionSequence
{
	
	// Called to get an object (used by the asset details panel)
	DECLARE_DELEGATE_RetVal(UObject*, OnGetAssetDelegate);

	/** A delegate which will create an auto-key handler. */
	DECLARE_DELEGATE_RetVal_TwoParams(FFrameNumber, OnGetNearestKeyDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FActActionTrackEditorBase>, OnCreateTrackEditorDelegate, TSharedRef<FActActionSequenceController>);

	/** A delegate that is executed when menu object is clicked. Unlike FExtender delegates we pass in the FGuid which exists even for deleted objects. */
	DECLARE_DELEGATE_TwoParams(OnBuildCustomContextMenuForGuidDelegate, FMenuBuilder&, FGuid);
	DECLARE_DELEGATE_TwoParams(OnScrubPositionChangedDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_TwoParams(OnViewRangeChangedDelegate, TRange<double>, EActActionViewRangeInterpolation)
	/** A delegate that is executed when adding menu content. */
	DECLARE_DELEGATE_TwoParams(OnGetAddMenuContentDelegate, FMenuBuilder& /*MenuBuilder*/, TSharedRef<FActActionSequenceController>);
	DECLARE_DELEGATE_TwoParams(OnSetMarkedFrameDelegate, int32, FFrameNumber)

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
}