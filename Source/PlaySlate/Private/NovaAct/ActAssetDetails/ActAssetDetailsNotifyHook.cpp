#include "ActAssetDetailsNotifyHook.h"

#include "PlaySlate.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/ActAnimation.h"
// #include "NovaAct/NovaActEditor.h"

FActAssetDetailsNotifyHook::~FActAssetDetailsNotifyHook()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActAssetDetailsNotifyHook::~FActAssetDetailsNotifyHook"));
}

void FActAssetDetailsNotifyHook::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	UE_LOG(LogNovaAct, Log, TEXT("PropertyChangedEvent : %s"), *PropertyChangedEvent.GetPropertyName().ToString());
	NovaDB::Trigger("ActAnimation");
	if(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UActAnimation, AnimSequence))
	{
		NovaDB::Trigger("ActAnimation/AnimSequence");
	}
	if(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UActAnimation, AnimBlueprint))
	{
		NovaDB::Trigger("ActAnimation/AnimBlueprint");
	}
}
