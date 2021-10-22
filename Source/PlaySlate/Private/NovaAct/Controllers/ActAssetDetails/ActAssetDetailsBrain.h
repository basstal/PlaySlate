#pragma once

class SActActionDetailsViewWidget;
class FNovaActEditor;

class FActAssetDetailsBrain : public TSharedFromThis<FActAssetDetailsBrain>
{
public:
	FActAssetDetailsBrain(const TSharedRef<FNovaActEditor>& InActActionSequenceEditor);
	~FActAssetDetailsBrain();

	/** 构造Widget */
	void MakeDetailsViewWidget();
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
