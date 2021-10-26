#include "ActAssetDetailsWidget.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActAnimation.h"

void SActAssetDetailsWidget::Construct(const FArguments& InArgs)
{
	// Create a property view
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	NotifyHook = MakeShareable(new FActAssetDetailsNotifyHook());
	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = true;
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.NotifyHook = NotifyHook.Get();
	}
	PropertyView = EditModule.CreateDetailView(DetailsViewArgs);

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

void SActAssetDetailsWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
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
			check(PropertyView);
			PropertyView->SetObjects(SelectedObjects);
		}
	}
}

UObject* SActAssetDetailsWidget::GetObjectToObserve() const
{
	TSharedPtr<TDataBindingUObject<UActAnimation>> DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	if (DB)
	{
		return DB->GetData();
	}
	return nullptr;
}
