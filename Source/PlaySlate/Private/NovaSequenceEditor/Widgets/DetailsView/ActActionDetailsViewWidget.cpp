#include "ActActionDetailsViewWidget.h"

void SActActionDetailsViewWidget::Construct(const FArguments& InArgs)
{
	OnGetAsset = InArgs._OnGetAsset;
	SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments(), true, true);
	InArgs._OnDetailsCreated.ExecuteIfBound(PropertyView.ToSharedRef());
}

EVisibility SActActionDetailsViewWidget::GetAssetDisplayNameVisibility() const
{
	return (GetObjectToObserve() != nullptr) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SActActionDetailsViewWidget::GetAssetDisplayName() const
{
	if (UObject* Object = GetObjectToObserve())
	{
		return FText::FromString(Object->GetName());
	}
	else
	{
		return FText::GetEmpty();
	}
}

UObject* SActActionDetailsViewWidget::GetObjectToObserve() const
{
	if (OnGetAsset.IsBound())
	{
		return OnGetAsset.Execute();
	}
	return nullptr;
}
