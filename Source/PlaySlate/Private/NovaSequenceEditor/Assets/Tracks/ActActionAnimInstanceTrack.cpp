#include "ActActionAnimInstanceTrack.h"

#include "PlaySlate.h"
#include "Utils/ActActionSequenceUtil.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "GameFramework/Character.h"
#include "Animation/AnimBlueprint.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"


#define LOCTEXT_NAMESPACE "ActAction"

FActActionAnimInstanceTrack::FActActionAnimInstanceTrack(const TSharedRef<FActActionSequenceController>& InSequenceController)
	: FActActionTrackEditorBase(InSequenceController)
{
}

//
// bool FActActionAnimInstanceTrack::SupportsSequence(UActActionSequence* InSequence) const
// {
// 	return true;
// }

void FActActionAnimInstanceTrack::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AssignAnimInstance", "Assign AnimInstance"),
		LOCTEXT("AssignAnimInstance", "Assign AnimInstance"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetTreeFolderOpen"),
		FUIAction(FExecuteAction::CreateRaw(this, &FActActionAnimInstanceTrack::AssignAnimInstance))
	);
}

TSharedRef<FActActionTrackEditorBase> FActActionAnimInstanceTrack::CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController)
{
	return MakeShareable(new FActActionAnimInstanceTrack(InSequenceController));
}

void FActActionAnimInstanceTrack::AssignAnimInstance()
{
	if (AnimInstanceSelectionDialog.IsValid())
	{
		AnimInstanceSelectionDialog->DestroyWindowImmediately();
	}
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = ActActionSequence::OnAssetSelectedDelegate::CreateSP(this, &FActActionAnimInstanceTrack::OnAssetSelected);
	AssetPickerConfig.OnAssetEnterPressed = ActActionSequence::OnAssetEnterPressedDelegate::CreateSP(this, &FActActionAnimInstanceTrack::OnAssetEnterPressed);
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
	AssetPickerConfig.Filter.bRecursiveClasses = true;
	AssetPickerConfig.Filter.ClassNames.Add(UAnimBlueprint::StaticClass()->GetFName());
	AssetPickerConfig.SaveSettingsName = TEXT("ActActionAnimMontageAssetPicker");

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	const FVector2D DefaultWindowSize(1152.0f, 648.0f);
	AnimInstanceSelectionDialog = SNew(SWindow)
    		.Title(LOCTEXT("TrackEditor", "Select AnimMontage Asset"))
    		.ClientSize(DefaultWindowSize);

	AnimInstanceSelectionDialog->SetContent(SNew(SBox)
	.HeightOverride(300)
	.WidthOverride(300)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
			[
				ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
			]
		]);
	FSlateApplication::Get().AddWindow(AnimInstanceSelectionDialog.ToSharedRef());
}

void FActActionAnimInstanceTrack::OnAssetSelected(const FAssetData& InAssetData)
{
	if (InAssetData.IsValid() && SequenceController.IsValid())
	{
		UE_LOG(LogActAction, Log, TEXT("InAssetData : %s"), *InAssetData.GetFullName());
		UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(InAssetData.GetAsset());
		SequenceController.Pin()->GetActActionSequenceEditor()->InitAnimBlueprint(AnimBlueprint);
		AnimInstanceSelectionDialog->RequestDestroyWindow();
	}
}


void FActActionAnimInstanceTrack::OnAssetEnterPressed(const TArray<FAssetData>& InAssetData)
{
	UE_LOG(LogActAction, Log, TEXT("InAssetData Num: %d"), InAssetData.Num());
	for (auto& AssetData : InAssetData)
	{
		UE_LOG(LogActAction, Log, TEXT("AssetData : %s"), *AssetData.GetFullName());
	}
}

#undef LOCTEXT_NAMESPACE
