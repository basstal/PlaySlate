#include "ActViewportTabWidget.h"

#include "ActViewport.h"

void SActViewportTabWidget::Construct(const FArguments& InArgs)
{
	// UICommandList = MakeShareable(new FUICommandList_Pinnable);
	//
	// PreviewScenePtr = StaticCastSharedRef<FAnimationEditorPreviewScene>(InPreviewScene);
	// AssetEditorToolkitPtr = InAssetEditorToolkit;
	// BlueprintEditorPtr = InArgs._BlueprintEditor;
	// bShowTimeline = InArgs._ShowTimeline;
	// bAlwaysShowTransformToolbar = InArgs._AlwaysShowTransformToolbar;
	// OnInvokeTab = InArgs._OnInvokeTab;
	// OnGetViewportText = InArgs._OnGetViewportText;
	//
	// // register delegates for change notifications
	// InPreviewScene->RegisterOnAnimChanged(FOnAnimChanged::CreateSP(this, &SAnimationEditorViewportTabBody::AnimChanged));
	// InPreviewScene->RegisterOnPreviewMeshChanged(FOnPreviewMeshChanged::CreateSP(this, &SAnimationEditorViewportTabBody::HandlePreviewMeshChanged));

	// const FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 9);
	//
	// FAnimViewportMenuCommands::Register();
	// FAnimViewportShowCommands::Register();
	// FAnimViewportLODCommands::Register();
	// FAnimViewportPlaybackCommands::Register();
	//
	// // Build toolbar widgets
	// UVChannelCombo = SNew(STextComboBox)
	// 	.OptionsSource(&UVChannels)
	// 	.Font(SmallLayoutFont)
	// 	.OnSelectionChanged(this, &SAnimationEditorViewportTabBody::ComboBoxSelectionChanged);
	//
	// PopulateSkinWeightProfileNames();
	//
	// SkinWeightCombo = SNew(SNameComboBox)
	// 	.OptionsSource(&SkinWeightProfileNames)
	// 	.InitiallySelectedItem(SkinWeightProfileNames.Num() > 0 ? SkinWeightProfileNames[0] : nullptr)
	// 	.OnComboBoxOpening(FOnComboBoxOpening::CreateLambda([this]()
	//                                      {
	// 	                                     // Retrieve currently selected value, and check whether or not it is still valid, it could be that a profile has been renamed or removed without updating the entries
	// 	                                     FName Name = SkinWeightCombo->GetSelectedItem().IsValid() ? *SkinWeightCombo->GetSelectedItem().Get() : NAME_None;
	// 	                                     PopulateSkinWeightProfileNames();
	// 	                                     const int32 Index = SkinWeightProfileNames.IndexOfByPredicate([Name](TSharedPtr<FName> SearchName) { return Name == *SearchName; });
	// 	                                     if (Index != INDEX_NONE)
	// 	                                     {
	// 		                                     SkinWeightCombo->SetSelectedItem(SkinWeightProfileNames[Index]);
	// 	                                     }
	//                                      }))
	// 	.OnSelectionChanged(SNameComboBox::FOnNameSelectionChanged::CreateLambda([WeakScenePtr = PreviewScenePtr](TSharedPtr<FName> SelectedProfile, ESelectInfo::Type SelectInfo)
	//                                      {
	// 	                                     // Apply the skin weight profile to the component, according to the selected the name, 
	// 	                                     if (WeakScenePtr.IsValid() && SelectedProfile.IsValid())
	// 	                                     {
	// 		                                     UDebugSkelMeshComponent* MeshComponent = WeakScenePtr.Pin()->GetPreviewMeshComponent();
	// 		                                     if (MeshComponent)
	// 		                                     {
	// 			                                     MeshComponent->ClearSkinWeightProfile();
	//
	// 			                                     if (*SelectedProfile != NAME_None)
	// 			                                     {
	// 				                                     MeshComponent->SetSkinWeightProfile(*SelectedProfile);
	// 			                                     }
	// 		                                     }
	// 	                                     }
	//                                      }));
	//
	// FAnimationEditorViewportRequiredArgs ViewportArgs(InPreviewScene, SharedThis(this), InAssetEditorToolkit, InViewportIndex);

	// ViewportWidget =;
	// .Extenders(InArgs._Extenders)
	// .ContextName(InArgs._ContextName)
	// .ShowShowMenu(InArgs._ShowShowMenu)
	// .ShowLODMenu(InArgs._ShowLODMenu)
	// .ShowPlaySpeedMenu(InArgs._ShowPlaySpeedMenu)
	// .ShowStats(InArgs._ShowStats)
	// .ShowFloorOptions(InArgs._ShowFloorOptions)
	// .ShowTurnTable(InArgs._ShowTurnTable)
	// .ShowPhysicsMenu(InArgs._ShowPhysicsMenu);

	TSharedPtr<SVerticalBox> ViewportContainer = nullptr;
	ChildSlot
	[
		SAssignNew(ViewportContainer, SVerticalBox)

		// Build our toolbar level toolbar
		+ SVerticalBox::Slot()
		.FillHeight(1)
		[
			SNew(SOverlay)

			// The viewport
			+ SOverlay::Slot()
			[
				SNew(SActViewport)
			]

			// // The 'dirty/in-error' indicator text in the bottom-right corner
			// +SOverlay::Slot()
			// .Padding(8)
			// .VAlign(VAlign_Bottom)
			// .HAlign(HAlign_Right)
			// [
			// 	SAssignNew(ViewportNotificationsContainer, SVerticalBox)
			// ]
		]
	];

	// if(bShowTimeline && ViewportContainer.IsValid())
	// {
	// 	ViewportContainer->AddSlot()
	// 	.AutoHeight()
	// 	[
	// 		SAssignNew(ScrubPanelContainer, SVerticalBox)
	// 		+SVerticalBox::Slot()
	// 		.AutoHeight()
	// 		[
	// 			SNew(SAnimationScrubPanel, GetPreviewScene())
	// 			.ViewInputMin(this, &SAnimationEditorViewportTabBody::GetViewMinInput)
	// 			.ViewInputMax(this, &SAnimationEditorViewportTabBody::GetViewMaxInput)
	// 			.bAllowZoom(true)
	// 		]
	// 	];
	//
	// 	UpdateScrubPanel(InPreviewScene->GetPreviewAnimationAsset());
	// }
	//
	// LevelViewportClient = ViewportWidget->GetViewportClient();
	//
	// TSharedRef<FAnimationViewportClient> AnimViewportClient = StaticCastSharedRef<FAnimationViewportClient>(LevelViewportClient.ToSharedRef());
	//
	// // Load the view mode from config
	// AnimViewportClient->SetViewMode(AnimViewportClient->ConfigOption->GetAssetEditorOptions(AssetEditorToolkitPtr.Pin()->GetEditorName()).ViewportConfigs[InViewportIndex].ViewModeIndex);
	// UpdateShowFlagForMeshEdges();
	//
	//
	// OnSetTurnTableMode(SelectedTurnTableMode);
	// OnSetTurnTableSpeed(SelectedTurnTableSpeed);
	//
	// BindCommands();
	//
	// PopulateNumUVChannels();
	// PopulateSkinWeightProfileNames();
	//
	// GetPreviewScene()->OnRecordingStateChanged().AddSP(this, &SAnimationEditorViewportTabBody::AddRecordingNotification);
	// if (GetPreviewScene()->GetPreviewMesh())
	// {
	// 	GetPreviewScene()->GetPreviewMesh()->OnPostMeshCached().AddSP(this, &SAnimationEditorViewportTabBody::UpdateSkinWeightSelection);
	// }
	//
	// AddPostProcessNotification();
	//
	// AddMinLODNotification();
	//
	// AddSkinWeightProfileNotification();
}
