#pragma once
#include "NovaAct/ActActionSequenceEditor.h"

class SActActionDetailsViewWidget;

class FActActionDetailsViewController : public TSharedFromThis<FActActionDetailsViewController>
{
public:
	FActActionDetailsViewController(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	~FActActionDetailsViewController();


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
