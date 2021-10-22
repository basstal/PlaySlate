#include "ActActionSequenceFactory.h"

#include "NovaAct/Assets/ActAnimation.h"

#define LOCTEXT_NAMESPACE "NovaAct"

UActActionSequenceFactory::UActActionSequenceFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UActAnimation::StaticClass();
}

bool UActActionSequenceFactory::ConfigureProperties()
{
	return Super::ConfigureProperties();
}

UObject* UActActionSequenceFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn, NAME_None);
}

UObject* UActActionSequenceFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UActAnimation* ActActionSequence = NewObject<UActAnimation>(InParent, InName, Flags);
	return ActActionSequence;
}

bool UActActionSequenceFactory::ShouldShowInNewMenu() const
{
	return true;
}


#undef LOCTEXT_NAMESPACE
