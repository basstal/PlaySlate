#include "ActAssetDetailsNotifyHook.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/NovaActEditor.h"
// #include "NovaAct/Widgets/ActAssetDetails/ActAssetDetailsWidget.h"

FActAssetDetailsNotifyHook::~FActAssetDetailsNotifyHook()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActAssetDetailsNotifyHook::~FActAssetDetailsNotifyHook"));
}

void FActAssetDetailsNotifyHook::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	UE_LOG(LogNovaAct, Log, TEXT("PropertyChangedEvent : %s"), *PropertyChangedEvent.GetPropertyName().ToString());
	auto DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	DB->Trigger();
}
