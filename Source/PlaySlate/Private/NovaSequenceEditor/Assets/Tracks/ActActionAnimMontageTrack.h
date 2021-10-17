#pragma once
#include "ActActionTrackEditorBase.h"

class FActActionAnimMontageTrack : public FActActionTrackEditorBase
{
public:
	FActActionAnimMontageTrack(const TSharedRef<FActActionSequenceController>& ActActionSequenceController);


	//~Begin FActActionTrackEditorBase interface
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	//~End FActActionTrackEditorBase interface
	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController);

	/** TODO: */
	void AssignAnimMontage();
	/** TODO: */
	void OnAssetSelected(const FAssetData& InAssetData);
	/** TODO: */
	void OnAssetEnterPressed(const TArray<FAssetData>& InAssetData);
protected:
	TSharedPtr<SWindow> AnimMontageSelectionDialog;
};
