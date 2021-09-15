#pragma once
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class UFactory;

class FAssetTypeActions_ActActionBlueprint : public FAssetTypeActions_Blueprint
{
public:
	//~Begin IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
	//~End IAssetTypeActions interface


	//~Begin FAssetTypeActions_Blueprint interface
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;
	//~End FAssetTypeActions_Blueprint interface

protected:
	bool ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const;
};
