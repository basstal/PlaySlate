#pragma once
#include "SSingleObjectDetailsPanel.h"
#include "Utils/ActActionSequenceUtil.h"

class SActActionDetailsViewWidget : public SSingleObjectDetailsPanel
{
public:
SLATE_BEGIN_ARGS(SActActionDetailsViewWidget)
		{
		}

		SLATE_ARGUMENT(ActActionSequence::OnGetAssetDelegate, OnGetAsset)

		SLATE_ARGUMENT(ActActionSequence::OnDetailsCreatedDelegate, OnDetailsCreated)

	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs);
	/** @return 是否显示名称 */
	EVisibility GetAssetDisplayNameVisibility() const;

	/** 获得显示的名称 */
	FText GetAssetDisplayName() const;

	// ~Begin SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;
	// ~End SSingleObjectDetailsPanel interface

protected:
	ActActionSequence::OnGetAssetDelegate OnGetAsset;
};
