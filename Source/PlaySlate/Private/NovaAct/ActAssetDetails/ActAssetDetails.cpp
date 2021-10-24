#include "ActAssetDetails.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Widgets/ActAssetDetails/ActActionDetailsViewWidget.h"

FActAssetDetails::FActAssetDetails() {}

FActAssetDetails::~FActAssetDetails()
{
	UE_LOG(LogActAction, Log, TEXT("FActAssetDetails::~FActAssetDetails"));
}

void FActAssetDetails::Init()
{
	ActActionDetailsViewWidget = SNew(SActActionDetailsViewWidget, SharedThis(this))
		.OnGetAsset(OnGetAssetDelegate::CreateLambda([this]()
		{
			auto DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
			return StaticCast<UObject*>(DB->GetData());
		}));
}

void FActAssetDetails::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	UE_LOG(LogActAction, Log, TEXT("PropertyChangedEvent : %s"), *PropertyChangedEvent.GetPropertyName().ToString());
	auto DB = NovaDB::Get<UActAnimation*>("ActAnimation");
	DB->Trigger();
}
