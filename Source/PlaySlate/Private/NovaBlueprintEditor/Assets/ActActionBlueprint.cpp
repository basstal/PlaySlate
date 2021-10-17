#include "ActActionBlueprint.h"

#if WITH_EDITOR
bool UActActionBlueprint::SupportedByDefaultBlueprintFactory() const
{
	return Super::SupportedByDefaultBlueprintFactory();
}

UActActionBlueprint* UActActionBlueprint::FindRootActionBlueprint(UActActionBlueprint* DerivedBlueprint)
{
	UActActionBlueprint* ParentBP = nullptr;

	for (UClass* ParentClass = DerivedBlueprint->ParentClass; ParentClass != UObject::StaticClass(); ParentClass = ParentClass->GetSuperClass())
	{
		if (UActActionBlueprint* TestBP = Cast<UActActionBlueprint>(ParentClass->ClassGeneratedBy))
		{
			ParentBP = TestBP;
		}
	}
	return ParentBP;
}
#endif
