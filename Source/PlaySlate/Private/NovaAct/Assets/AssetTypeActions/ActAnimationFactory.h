#pragma once


#include "ActAnimationFactory.generated.h"

UCLASS(MinimalAPI)
class UActAnimationFactory : public UFactory
{
	GENERATED_BODY()
public:
	UActAnimationFactory();

	//~Begin UFactory interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual bool ShouldShowInNewMenu() const override;
	//~End UFactory interface
};
