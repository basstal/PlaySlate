#include "ActAnimationFactory.h"

#include "NovaAct/Assets/ActAnimation.h"

#define LOCTEXT_NAMESPACE "NovaAct"

UActAnimationFactory::UActAnimationFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UActAnimation::StaticClass();
}

bool UActAnimationFactory::ConfigureProperties()
{
	return Super::ConfigureProperties();
}

UObject* UActAnimationFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn, NAME_None);
}

UObject* UActAnimationFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UActAnimation* ActActionSequence = NewObject<UActAnimation>(InParent, InName, Flags);
	return ActActionSequence;
}

bool UActAnimationFactory::ShouldShowInNewMenu() const
{
	return true;
}


#undef LOCTEXT_NAMESPACE
