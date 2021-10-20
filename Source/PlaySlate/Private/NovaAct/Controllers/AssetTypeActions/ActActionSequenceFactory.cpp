#include "ActActionSequenceFactory.h"

#include "NovaAct/Assets/ActActionSequence.h"

#define LOCTEXT_NAMESPACE "NovaAct"

UActActionSequenceFactory::UActActionSequenceFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UActActionSequence::StaticClass();
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
	UActActionSequence* ActActionSequence = NewObject<UActActionSequence>(InParent, InName, Flags);
	return ActActionSequence;
}

bool UActActionSequenceFactory::ShouldShowInNewMenu() const
{
	return true;
}


#undef LOCTEXT_NAMESPACE
