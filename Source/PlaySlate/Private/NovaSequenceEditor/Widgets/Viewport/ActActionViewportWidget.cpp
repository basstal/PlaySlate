#include "ActActionViewportWidget.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionViewportClient.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewSceneController.h"

void SActActionViewportWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
{
	ActActionSequenceEditor = InActActionSequenceEditor;

	SEditorViewport::Construct(
		SEditorViewport::FArguments()
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
		.AddMetaData<FTagMetaData>(TEXT("ActAction.Viewport"))
	);

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
				ViewportWidget.ToSharedRef()
			]

			// The 'dirty/in-error' indicator text in the bottom-right corner
			+ SOverlay::Slot()
			  .Padding(8)
			  .VAlign(VAlign_Bottom)
			  .HAlign(HAlign_Right)
			[
				SAssignNew(ViewportNotificationsContainer, SVerticalBox)
			]
		]

	];
}

SActActionViewportWidget::~SActActionViewportWidget()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionViewportWidget::~SActActionViewportWidget"));
	// Client.Get();
	// The preview scene that we are viewing
	// PreviewScenePtr.Reset();

	// ViewportClient.Reset();
	/** Box that contains notifications */
	// ViewportNotificationsContainer.Reset();
}

int32 SActActionViewportWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return SEditorViewport::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

TSharedRef<FEditorViewportClient> SActActionViewportWidget::MakeEditorViewportClient()
{
	// Create an ActAction viewport client
	return GetPreviewScenePtr()->MakeViewportClient(SharedThis(this)).ToSharedRef();
}

TSharedPtr<FActActionPreviewSceneController> SActActionViewportWidget::GetPreviewScenePtr() const
{
	if (ActActionSequenceEditor.IsValid())
	{
		return ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController();
	}
	return nullptr;
}
