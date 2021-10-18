#pragma once
#include "SSingleObjectDetailsPanel.h"
#include "Utils/ActActionSequenceUtil.h"

class FActActionSequenceEditor;

class SActActionDetailsViewWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionDetailsViewWidget)
		{
		}

		SLATE_ARGUMENT(ActActionSequence::OnGetAssetDelegate, OnGetAsset)
		SLATE_ARGUMENT(ActActionSequence::OnDetailsCreatedDelegate, OnDetailsCreated)
		SLATE_ARGUMENT(TSharedPtr<class FUICommandList>, HostCommandList)
		SLATE_ARGUMENT(TSharedPtr<class FTabManager>, HostTabManager)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	
	//~Begin SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~End SWidget interface
	
	/** @return 是否显示名称 */
	EVisibility GetAssetDisplayNameVisibility() const;

	/** 获得显示的名称 */
	FText GetAssetDisplayName() const;

	/** 获得当前Details面板监听的对象 */
	UObject* GetObjectToObserve() const;

protected:
	// Property viewing widget
	TSharedPtr<class IDetailsView> PropertyView;

	// Cached object view
	TWeakObjectPtr<UObject> LastObservedObject;

	ActActionSequence::OnGetAssetDelegate OnGetAsset;
};
