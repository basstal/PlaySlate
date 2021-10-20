#pragma once

#include "Factories/Factory.h"

#include "ActActionBlueprintFactory.generated.h"

class UActActionLogic;

UCLASS(MinimalAPI)
class UActActionBlueprintFactory : public UFactory
{
	GENERATED_BODY()
public:
	UActActionBlueprintFactory();

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBlueprintType> BlueprintType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UActActionLogic> ParentClass;

	//~Begin UFactory interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	//~End UFactory interface
};
