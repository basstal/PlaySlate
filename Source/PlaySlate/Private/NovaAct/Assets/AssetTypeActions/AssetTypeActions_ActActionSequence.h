#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_ActActionSequence : public FAssetTypeActions_Base
{
public:
	//~Begin IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	//~End IAssetTypeActions interface
};
