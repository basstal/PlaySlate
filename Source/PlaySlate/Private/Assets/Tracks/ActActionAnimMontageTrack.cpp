#include "ActActionAnimMontageTrack.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Animation/AnimMontage.h"
#include "Editor/ActActionSequenceController.h"
#include "Editor/ActActionSequenceDisplayNode.h"
#include "Utils/ActActionSequenceUtil.h"
#include "PlaySlate/PlaySlate.h"

#define LOCTEXT_NAMESPACE "ActAction"

class FContentBrowserModule;

TSharedRef<FActActionTrackEditorBase> FActActionAnimMontageTrack::CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController)
{
	return MakeShareable(new FActActionAnimMontageTrack(InSequenceController));
}

void FActActionAnimMontageTrack::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AssignAnimMontage", "Assign AnimMontage"),
		LOCTEXT("AssignAnimMontage", "Assign AnimMontage"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetTreeFolderOpen"),
		FUIAction(FExecuteAction::CreateRaw(this, &FActActionAnimMontageTrack::AssignAnimMontage))
	);
}



void FActActionAnimMontageTrack::AssignAnimMontage()
{
	if (AnimMontageSelectionDialog.IsValid())
	{
		AnimMontageSelectionDialog->DestroyWindowImmediately();
	}
	FAssetPickerConfig AssetPickerConfig;
	{
		AssetPickerConfig.OnAssetSelected = ActActionSequence::OnAssetSelectedDelegate::CreateSP(this, &FActActionAnimMontageTrack::OnAssetSelected);
		AssetPickerConfig.OnAssetEnterPressed = ActActionSequence::OnAssetEnterPressedDelegate::CreateSP(this, &FActActionAnimMontageTrack::OnAssetEnterPressed);
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
		AssetPickerConfig.Filter.bRecursiveClasses = true;
		AssetPickerConfig.Filter.ClassNames.Add(UAnimMontage::StaticClass()->GetFName());
		AssetPickerConfig.SaveSettingsName = TEXT("ActActionAnimMontageAssetPicker");
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	const FVector2D DefaultWindowSize(1152.0f, 648.0f);
	AnimMontageSelectionDialog = SNew(SWindow)
			.Title(LOCTEXT("TrackEditor", "Select AnimMontage Asset"))
			.ClientSize(DefaultWindowSize);

	AnimMontageSelectionDialog->SetContent(SNew(SBox)
	.HeightOverride(300)
	.WidthOverride(300)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
			[
				ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
			]
		]);
	FSlateApplication::Get().AddWindow(AnimMontageSelectionDialog.ToSharedRef());
}


void FActActionAnimMontageTrack::OnAssetSelected(const FAssetData& InAssetData)
{
	if (InAssetData.IsValid())
	{
		UE_LOG(LogActAction, Log, TEXT("InAssetData : %s"), *InAssetData.GetFullName());
		if (SequenceController.IsValid())
		{
			UAnimMontage* AnimMontage = Cast<UAnimMontage>(InAssetData.GetAsset());
			if (AnimMontage)
			{
				UE_LOG(LogActAction, Log, TEXT("AnimMontage : %s"), *AnimMontage->GetName());
				TSharedPtr<FActActionSequenceController> SequenceControllerPtr = SequenceController.Pin();
				SequenceControllerPtr->GetActActionSequencePtr()->EditAnimMontage = AnimMontage;
				SequenceControllerPtr->AddRootNodes(MakeShareable(new FActActionSequenceDisplayNode()));
			}
			AnimMontageSelectionDialog->RequestDestroyWindow();
		}
	}
}


void FActActionAnimMontageTrack::OnAssetEnterPressed(const TArray<FAssetData>& InAssetData)
{
	UE_LOG(LogActAction, Log, TEXT("InAssetData Num: %d"), InAssetData.Num());
	for (auto& AssetData : InAssetData)
	{
		UE_LOG(LogActAction, Log, TEXT("AssetData : %s"), *AssetData.GetFullName());
	}
}

#undef LOCTEXT_NAMESPACE
