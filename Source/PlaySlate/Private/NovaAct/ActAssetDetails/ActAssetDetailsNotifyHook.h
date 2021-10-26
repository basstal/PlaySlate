#pragma once

class SActAssetDetailsWidget;
class FNovaActEditor;

class FActAssetDetailsNotifyHook : public FNotifyHook
{
public:
	virtual ~FActAssetDetailsNotifyHook();
	
	//~Begin FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
	//~End FNotifyHook interface
};
