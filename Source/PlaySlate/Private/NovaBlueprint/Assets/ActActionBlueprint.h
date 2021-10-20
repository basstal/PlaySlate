#pragma once

#include "ActActionBlueprint.generated.h"

UCLASS(BlueprintType)
class UActActionBlueprint : public UBlueprint
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	//~Begin UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override;
	//~End UBlueprint interface


	static UActActionBlueprint* FindRootActionBlueprint(UActActionBlueprint* DerivedBlueprint);

#endif
};
