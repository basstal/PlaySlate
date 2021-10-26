// #pragma once
//
// #include "ActActionTrackEditorBase.h"
//
// class FActActionAnimInstanceTrack : public FActActionTrackEditorBase
// {
// public:
// 	FActActionAnimInstanceTrack(const TSharedRef<FActEventTimeline>& InSequenceController);
//
// 	//~Begin FActActionTrackEditorBase interface
// 	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
// 	//~End FActActionTrackEditorBase interface
//
// 	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<FActEventTimeline> InSequenceController);
//
// 	/**
// 	 * Delegate callback
// 	 */
// 	void AssignAnimInstance();
// 	/**
// 	 * Delegate callback
// 	 */
// 	void OnAssetSelected(const FAssetData& InAssetData);
// 	/**
// 	 * Delegate callback
// 	 */
// 	void OnAssetEnterPressed(const TArray<FAssetData>& InAssetData);
// protected:
// 	TSharedPtr<SWindow> AnimInstanceSelectionDialog;
// };
