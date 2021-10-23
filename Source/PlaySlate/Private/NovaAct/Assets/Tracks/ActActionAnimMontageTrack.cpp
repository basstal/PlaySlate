#include "ActActionAnimMontageTrack.h"

#include "PlaySlate.h"
#include "Common/NovaStruct.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Animation/AnimMontage.h"

#define LOCTEXT_NAMESPACE "NovaAct"

class FContentBrowserModule;


FActActionAnimMontageTrack::FActActionAnimMontageTrack(const TSharedRef<FActEventTimeline>& ActActionSequenceController)
	: FActActionTrackEditorBase(ActActionSequenceController)
{
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


TSharedRef<FActActionTrackEditorBase> FActActionAnimMontageTrack::CreateTrackEditor(TSharedRef<FActEventTimeline> InSequenceController)
{
	return MakeShareable(new FActActionAnimMontageTrack(InSequenceController));
}

void FActActionAnimMontageTrack::AssignAnimMontage()
{
	if (AnimMontageSelectionDialog.IsValid())
	{
		AnimMontageSelectionDialog->DestroyWindowImmediately();
	}
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = OnAssetSelectedDelegate::CreateSP(this, &FActActionAnimMontageTrack::OnAssetSelected);
	AssetPickerConfig.OnAssetEnterPressed = OnAssetEnterPressedDelegate::CreateSP(this, &FActActionAnimMontageTrack::OnAssetEnterPressed);
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
	AssetPickerConfig.Filter.bRecursiveClasses = true;
	AssetPickerConfig.Filter.ClassNames.Add(UAnimMontage::StaticClass()->GetFName());
	AssetPickerConfig.SaveSettingsName = TEXT("ActActionAnimMontageAssetPicker");

	const FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

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
	if (ActActionSequenceController.IsValid() && InAssetData.IsValid())
	{
		UE_LOG(LogActAction, Log, TEXT("InAssetData : %s"), *InAssetData.GetFullName());
		// ** TODO:暂时不支持Montage资源
		AnimMontageSelectionDialog->RequestDestroyWindow();
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
