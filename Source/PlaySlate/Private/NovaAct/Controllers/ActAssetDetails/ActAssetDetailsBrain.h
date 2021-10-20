#pragma once

class SActActionDetailsViewWidget;
class FActActionSequenceEditor;

class FActAssetDetailsBrain : public TSharedFromThis<FActAssetDetailsBrain>
{
public:
	FActAssetDetailsBrain(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	~FActAssetDetailsBrain();

	/** 构造Widget */
	void MakeDetailsViewWidget();
protected:
	/** Sequence Editor */
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
	/** Widget */
	TSharedPtr<SActActionDetailsViewWidget> ActActionDetailsViewWidget;
public:
	TSharedRef<SActActionDetailsViewWidget> GetActActionDetailsViewWidget() const
	{
		return ActActionDetailsViewWidget.ToSharedRef();
	}
};
