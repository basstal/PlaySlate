// #include "ActActionAnimInstanceTrack.h"
//
// #include "PlaySlate.h"
// #include "NovaAct/ActEventTimeline/ActEventTimeline.h"
//
// #include "ContentBrowserModule.h"
// #include "IContentBrowserSingleton.h"
// #include "GameFramework/Character.h"
// #include "Animation/AnimBlueprint.h"
// #include "NovaAct/NovaActEditor.h"
//
//
// #define LOCTEXT_NAMESPACE "NovaAct"
//
// FActActionAnimInstanceTrack::FActActionAnimInstanceTrack(const TSharedRef<FActEventTimeline>& InSequenceController)
// 	: FActActionTrackEditorBase(InSequenceController)
// {
// }
//
// void FActActionAnimInstanceTrack::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
// {
// 	MenuBuilder.AddMenuEntry(
// 		LOCTEXT("AssignAnimInstance", "Assign AnimInstance"),
// 		LOCTEXT("AssignAnimInstance", "Assign AnimInstance"),
// 		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetTreeFolderOpen"),
// 		FUIAction(FExecuteAction::CreateRaw(this, &FActActionAnimInstanceTrack::AssignAnimInstance))
// 	);
// }
//
// TSharedRef<FActActionTrackEditorBase> FActActionAnimInstanceTrack::CreateTrackEditor(TSharedRef<FActEventTimeline> InSequenceController)
// {
// 	return MakeShareable(new FActActionAnimInstanceTrack(InSequenceController));
// }
//
// void FActActionAnimInstanceTrack::AssignAnimInstance()
// {
// 	if (AnimInstanceSelectionDialog.IsValid())
// 	{
// 		AnimInstanceSelectionDialog->DestroyWindowImmediately();
// 	}
// 	FAssetPickerConfig AssetPickerConfig;
// 	AssetPickerConfig.OnAssetSelected = OnAssetSelectedDelegate::CreateSP(this, &FActActionAnimInstanceTrack::OnAssetSelected);
// 	AssetPickerConfig.OnAssetEnterPressed = OnAssetEnterPressedDelegate::CreateSP(this, &FActActionAnimInstanceTrack::OnAssetEnterPressed);
// 	AssetPickerConfig.bAllowNullSelection = false;
// 	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
// 	AssetPickerConfig.Filter.bRecursiveClasses = true;
// 	AssetPickerConfig.Filter.ClassNames.Add(UAnimBlueprint::StaticClass()->GetFName());
// 	AssetPickerConfig.SaveSettingsName = TEXT("ActActionAnimMontageAssetPicker");
//
// 	const FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
//
// 	const FVector2D DefaultWindowSize(1152.0f, 648.0f);
// 	AnimInstanceSelectionDialog = SNew(SWindow)
//     .Title(LOCTEXT("TrackEditor", "Select AnimMontage Asset"))
//     .ClientSize(DefaultWindowSize);
//
// 	AnimInstanceSelectionDialog->SetContent(SNew(SBox)
// 	.HeightOverride(300)
// 	.WidthOverride(300)
// 		[
// 			SNew(SBorder)
// 			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
// 			[
// 				ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
// 			]
// 		]);
// 	FSlateApplication::Get().AddWindow(AnimInstanceSelectionDialog.ToSharedRef());
// }
//
// void FActActionAnimInstanceTrack::OnAssetSelected(const FAssetData& InAssetData)
// {
// 	if (InAssetData.IsValid() && ActActionSequenceController.IsValid())
// 	{
// 		UE_LOG(LogNovaAct, Log, TEXT("InAssetData : %s"), *InAssetData.GetFullName());
// 		// ** 暂不支持以此方式添加AnimBlueprint
// 		AnimInstanceSelectionDialog->RequestDestroyWindow();
// 	}
// }
//
//
// void FActActionAnimInstanceTrack::OnAssetEnterPressed(const TArray<FAssetData>& InAssetData)
// {
// 	UE_LOG(LogNovaAct, Log, TEXT("InAssetData Num: %d"), InAssetData.Num());
// 	for (auto& AssetData : InAssetData)
// 	{
// 		UE_LOG(LogNovaAct, Log, TEXT("AssetData : %s"), *AssetData.GetFullName());
// 	}
// }
//
// #undef LOCTEXT_NAMESPACE
