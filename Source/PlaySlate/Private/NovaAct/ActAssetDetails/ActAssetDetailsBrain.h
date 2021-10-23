#pragma once

class SActActionDetailsViewWidget;
class FNovaActEditor;

class FActAssetDetails : public TSharedFromThis<FActAssetDetails>
{
public:
	FActAssetDetails(const TSharedRef<FNovaActEditor>& InActActionSequenceEditor);
	~FActAssetDetails();

	/** 构造Widget */
	void Init();
protected:
	/** Sequence Editor */
	TWeakPtr<FNovaActEditor> ActActionSequenceEditor;
	/** Widget */
	TSharedPtr<SActActionDetailsViewWidget> ActActionDetailsViewWidget;
public:
	TSharedRef<SActActionDetailsViewWidget> GetActActionDetailsViewWidget() const
	{
		return ActActionDetailsViewWidget.ToSharedRef();
	}
};
