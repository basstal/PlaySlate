#pragma once

class SActActionDetailsViewWidget;
class FNovaActEditor;

class FActAssetDetails : public TSharedFromThis<FActAssetDetails>, public FNotifyHook
{
public:
	FActAssetDetails();
	virtual ~FActAssetDetails();

	/** 构造Widget */
	void Init();

	//~Begin FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
	//~End FNotifyHook interface
protected:
	/** Widget */
	TSharedPtr<SActActionDetailsViewWidget> ActActionDetailsViewWidget;
public:
	TSharedRef<SActActionDetailsViewWidget> GetActActionDetailsViewWidget() const
	{
		return ActActionDetailsViewWidget.ToSharedRef();
	}
};
