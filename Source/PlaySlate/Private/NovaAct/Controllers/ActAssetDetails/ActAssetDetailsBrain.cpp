#include "ActAssetDetailsBrain.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Widgets/ActAssetDetails/ActActionDetailsViewWidget.h"

FActAssetDetailsBrain::FActAssetDetailsBrain(const TSharedRef<FNovaActEditor>& InActActionSequenceEditor)
	: ActActionSequenceEditor(InActActionSequenceEditor)
{
}

FActAssetDetailsBrain::~FActAssetDetailsBrain()
{
	UE_LOG(LogActAction, Log, TEXT("FActAssetDetailsBrain::~FActAssetDetailsBrain"));
}

void FActAssetDetailsBrain::MakeDetailsViewWidget()
{
	ActActionDetailsViewWidget = SNew(SActActionDetailsViewWidget, ActActionSequenceEditor.Pin().ToSharedRef())
		.OnGetAsset(ActActionSequence::OnGetAssetDelegate::CreateLambda([this]()
		{
			return StaticCast<UObject*>(ActActionSequenceEditor.Pin()->GetActActionSequence());
		}));
}
