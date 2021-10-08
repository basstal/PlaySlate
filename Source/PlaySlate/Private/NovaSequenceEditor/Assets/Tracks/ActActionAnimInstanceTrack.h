#pragma once

#include "ActActionTrackEditorBase.h"

class FActActionAnimInstanceTrack : public FActActionTrackEditorBase
{
public:
	FActActionAnimInstanceTrack(const TSharedRef<FActActionSequenceController>& InSequenceController);

	//~Begin FActActionTrackEditorBase interface
	// virtual bool SupportsSequence(UActActionSequence* InSequence) const override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	//~End FActActionTrackEditorBase interface


	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController);

protected:
	/**
	 * Delegate callback
	 */
	void AssignAnimInstance();
	/**
	 * Delegate callback
	 */
	void OnAssetSelected(const FAssetData& InAssetData);
	/**
	 * Delegate callback
	 */
	void OnAssetEnterPressed(const TArray<FAssetData>& InAssetData);

protected:
	TSharedPtr<SWindow> AnimInstanceSelectionDialog;
};
