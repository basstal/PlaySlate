﻿#pragma once

#include "ActActionBlueprint.h"
#include "Factories/Factory.h"

#include "ActActionBlueprintFactory.generated.h"

UCLASS(MinimalAPI)
class UActActionBlueprintFactory : public UFactory
{
	GENERATED_BODY()
public:
	UActActionBlueprintFactory();

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBlueprintType> BlueprintType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UActActionBlueprint> ParentClass;

	//~Begin UFactory interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	//~End UFactory interface
};
