#pragma once

#include "Common/NovaDelegate.h"
#include "NovaAct/ActAssetDetails/ActAssetDetailsNotifyHook.h"

#include "SSingleObjectDetailsPanel.h"

class FNovaActEditor;

using namespace NovaDelegate;

class SActAssetDetailsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActAssetDetailsWidget) { }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	//~Begin SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~End SWidget interface

	/** @return 获得当前Details面板监听的对象 */
	UObject* GetObjectToObserve() const;

protected:
	TSharedPtr<FActAssetDetailsNotifyHook> NotifyHook;
	TSharedPtr<class IDetailsView> PropertyView;// Property viewing widget
	TWeakObjectPtr<UObject> LastObservedObject; // Cached object view
};
