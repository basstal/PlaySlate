#pragma once
#include "EdGraphSchema_K2.h"

#include "ActActionBlueprintGraphSchema.generated.h"

UCLASS(MinimalAPI)
class UActActionBlueprintGraphSchema : public UEdGraphSchema_K2
{
	GENERATED_BODY()
public:
	//~Begin UEdGraphSchema interface
	virtual bool ShouldAlwaysPurgeOnModification() const override;
	//~End UEdGraphSchema interface
};
