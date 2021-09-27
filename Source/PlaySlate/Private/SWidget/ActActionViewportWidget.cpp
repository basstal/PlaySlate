#include "ActActionViewportWidget.h"

#include "Editor/ActActionSequenceEditor.h"
#include "Editor/ActActionViewportClient.h"

void SActActionViewportWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InSequenceEditor, const TSharedRef<FActActionPreviewScene>& InPreviewScene)
{
	PreviewScenePtr = InPreviewScene;
	SequenceEditor = InSequenceEditor;

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
		+SVerticalBox::Slot()
		.FillHeight(1)
		[
			SNew(SOverlay)

			// The viewport
			+SOverlay::Slot()
			[
				ViewportWidget.ToSharedRef()
			]

			// The 'dirty/in-error' indicator text in the bottom-right corner
			+SOverlay::Slot()
			.Padding(8)
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			[
				SAssignNew(ViewportNotificationsContainer, SVerticalBox)
			]
		]
		
	];
}

int32 SActActionViewportWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return SEditorViewport::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

TSharedRef<FEditorViewportClient> SActActionViewportWidget::MakeEditorViewportClient()
{
	// Create an ActAction viewport client
	ViewportClient = MakeShareable(new FActActionViewportClient(GetPreviewScenePtr().ToSharedRef(), SharedThis(this), SequenceEditor.Pin().ToSharedRef()));
	return ViewportClient.ToSharedRef();
}
