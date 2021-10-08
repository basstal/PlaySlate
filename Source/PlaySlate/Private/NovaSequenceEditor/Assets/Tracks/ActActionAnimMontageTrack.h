#pragma once
#include "ActActionTrackEditorBase.h"

class FActActionAnimMontageTrack : public FActActionTrackEditorBase
{
public:
	FActActionAnimMontageTrack(const TSharedRef<FActActionSequenceController>& ActActionSequenceController)
		: FActActionTrackEditorBase(ActActionSequenceController)
	{
	}

	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	void AssignAnimMontage();
	void OnAssetSelected(const FAssetData& InAssetData);
	void OnAssetEnterPressed(const TArray<FAssetData>& InAssetData);
	static TSharedRef<FActActionTrackEditorBase> CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController);
protected:
	TSharedPtr<SWindow> AnimMontageSelectionDialog;
};
