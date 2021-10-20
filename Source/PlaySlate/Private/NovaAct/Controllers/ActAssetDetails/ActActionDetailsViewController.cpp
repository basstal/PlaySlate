#include "ActActionDetailsViewController.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Assets/ActActionSequence.h"
#include "NovaAct/ActActionSequenceEditor.h"
#include "NovaAct/Widgets/ActAssetDetails/ActActionDetailsViewWidget.h"

FActActionDetailsViewController::FActActionDetailsViewController(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
	: ActActionSequenceEditor(InActActionSequenceEditor)
{
}

FActActionDetailsViewController::~FActActionDetailsViewController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionDetailsViewController::~FActActionDetailsViewController"));
}

void FActActionDetailsViewController::MakeDetailsViewWidget()
{
	ActActionDetailsViewWidget = SNew(SActActionDetailsViewWidget, ActActionSequenceEditor.Pin().ToSharedRef())
		.OnGetAsset(ActActionSequence::OnGetAssetDelegate::CreateLambda([this]()
		{
			return StaticCast<UObject*>(ActActionSequenceEditor.Pin()->GetActActionSequence());
		}));
}
