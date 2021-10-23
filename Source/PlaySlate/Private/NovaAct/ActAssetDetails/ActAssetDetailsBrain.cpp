#include "ActAssetDetailsBrain.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Widgets/ActAssetDetails/ActActionDetailsViewWidget.h"

FActAssetDetails::FActAssetDetails(const TSharedRef<FNovaActEditor>& InActActionSequenceEditor)
	: ActActionSequenceEditor(InActActionSequenceEditor)
{}

FActAssetDetails::~FActAssetDetails()
{
	UE_LOG(LogActAction, Log, TEXT("FActAssetDetails::~FActAssetDetails"));
}

void FActAssetDetails::Init()
{
	ActActionDetailsViewWidget = SNew(SActActionDetailsViewWidget, ActActionSequenceEditor.Pin().ToSharedRef())
		.OnGetAsset(OnGetAssetDelegate::CreateLambda([this]()
		{
			auto DB = NovaDB::GetOrCreate<UActAnimation*>("ActAnimation");
			return StaticCast<UObject*>(DB->GetData());
		}));
}
