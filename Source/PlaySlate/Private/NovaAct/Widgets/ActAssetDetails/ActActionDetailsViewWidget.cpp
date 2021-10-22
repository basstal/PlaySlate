#include "ActActionDetailsViewWidget.h"
#include "NovaAct/NovaActEditor.h"

void SActActionDetailsViewWidget::Construct(const FArguments& InArgs, const TSharedRef<FNovaActEditor>& InActActionSequenceEditor)
{
	OnGetAsset = InArgs._OnGetAsset;

	// Create a property view
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.HostCommandList = InArgs._HostCommandList;
	DetailsViewArgs.HostTabManager = InArgs._HostTabManager;
	DetailsViewArgs.NotifyHook = &InActActionSequenceEditor.Get();
	PropertyView = EditModule.CreateDetailView(DetailsViewArgs);
	InArgs._OnDetailsCreated.ExecuteIfBound(PropertyView.ToSharedRef());

	// Create the border that all of the content will get stuffed into
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .FillHeight(1.0f)
		  .Padding(3.0f, 2.0f)
		[
			PropertyView.ToSharedRef()
		]
	];
}

void SActActionDetailsViewWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	UObject* CurrentObject = GetObjectToObserve();
	if (LastObservedObject.Get() != CurrentObject)
	{
		LastObservedObject = CurrentObject;

		TArray<UObject*> SelectedObjects;
		if (CurrentObject)
		{
			SelectedObjects.Add(CurrentObject);
		}

		if (FSlateApplication::IsInitialized())
		{
			check(PropertyView.IsValid());
			PropertyView->SetObjects(SelectedObjects);
		}
	}
}

EVisibility SActActionDetailsViewWidget::GetAssetDisplayNameVisibility() const
{
	return (GetObjectToObserve() != nullptr) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SActActionDetailsViewWidget::GetAssetDisplayName() const
{
	if (const UObject* Object = GetObjectToObserve())
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
