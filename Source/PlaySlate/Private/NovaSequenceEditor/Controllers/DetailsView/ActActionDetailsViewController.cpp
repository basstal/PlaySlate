#include "ActActionDetailsViewController.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Assets/ActActionSequence.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Widgets/DetailsView/ActActionDetailsViewWidget.h"

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
