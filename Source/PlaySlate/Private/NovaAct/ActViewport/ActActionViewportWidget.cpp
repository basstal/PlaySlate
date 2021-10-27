#include "ActActionViewportWidget.h"

#include "PlaySlate.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/ActViewport/ActActionViewportClient.h"
#include "NovaAct/ActViewport/ActViewportPreviewScene.h"

void SActActionViewportWidget::Construct(const FArguments& InArgs, const TSharedRef<FActViewportPreviewScene>& InActActionPreviewSceneController)
{
	ActActionPreviewSceneController = InActActionPreviewSceneController;

	SEditorViewport::Construct(
		SEditorViewport::FArguments()
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
		.AddMetaData<FTagMetaData>(TEXT("ActAction.Viewport"))
	);

	ChildSlot
	[
		SNew(SVerticalBox)

		// Build our toolbar level toolbar
		+ SVerticalBox::Slot()
		.FillHeight(1)
		[
			SNew(SOverlay)

			// The viewport
			+ SOverlay::Slot()
			[
				ViewportWidget.ToSharedRef()
			]

			// The 'dirty/in-error' indicator text in the bottom-right corner
			+ SOverlay::Slot()
			  .Padding(8)
			  .VAlign(VAlign_Bottom)
			  .HAlign(HAlign_Right)
			[
				SNew(SVerticalBox)
			]
		]

	];
}

SActActionViewportWidget::~SActActionViewportWidget()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActActionViewportWidget::~SActActionViewportWidget"));
}

TSharedRef<FEditorViewportClient> SActActionViewportWidget::MakeEditorViewportClient()
{
	return ActActionPreviewSceneController.Pin()->MakeViewportClient().ToSharedRef();
}
