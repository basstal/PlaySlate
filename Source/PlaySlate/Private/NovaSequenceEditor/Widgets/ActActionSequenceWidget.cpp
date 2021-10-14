#include "ActActionSequenceWidget.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTreeView.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/TimeSlider/ActActionTimeSliderWidget.h"
#include "NovaSequenceEditor/Widgets/TimeSlider/ActActionSequenceSectionOverlayWidget.h"

#include "FrameNumberNumericInterface.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "SEditorHeaderButton.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceTreeViewNode.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

SActActionSequenceWidget::~SActActionSequenceWidget()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceWidget::~SActActionSequenceWidget"));
}

void SActActionSequenceWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController)
{
	ColumnFillCoefficients[0] = 0.4f;
	ColumnFillCoefficients[1] = 0.6f;
	ActActionSequenceController = InSequenceController;

	TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	TSharedRef<FActActionSequenceTreeViewNode> ActActionSequenceTreeViewNode = InSequenceController->GetActActionSequenceTreeViewRoot();
	ActActionSequenceTreeViewNode->MakeActActionSequenceTreeView(ScrollBar);
	TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	ActActionSequenceTreeViewNode->MakeActActionSequenceTreeViewPinned(PinnedAreaScrollBar);

	const FMargin ResizeBarPadding(4.0f, 0, 0, 0);
	TAttribute<float> FillCoefficient_0, FillCoefficient_1;
	FillCoefficient_0.Bind(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceWidget::GetColumnFillCoefficient, 0));
	FillCoefficient_1.Bind(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceWidget::GetColumnFillCoefficient, 1));

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SSplitter).Orientation(Orient_Horizontal)
			+ SSplitter::Slot().Value(0.9f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SGridPanel).FillRow(2, 1.0f).FillColumn(0, FillCoefficient_0).FillColumn(1, FillCoefficient_1)
					// ** TODO:暂缺工具栏

					+ SGridPanel::Slot(0, 1)
					[
						SNew(SBorder).BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSpacer)
						]
					]

					// ** Outliner Search Box
					+ SGridPanel::Slot(0, 1, SGridPanel::Layer(10))
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.Padding(FMargin(0.6f, 0.3f))
						.Clipping(EWidgetClipping::ClipToBounds)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FMargin(0, 0, 0.3f, 0))
							[
								SNew(SEditorHeaderButton)
								.OnGetMenuContent(this, &SActActionSequenceWidget::MakeAddMenu)
								.Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
								.Text(LOCTEXT("Track", "Track"))
								.IsEnabled_Lambda([=]()
								{
									return ActActionSequenceController.Pin().IsValid();
								})
							]

							+ SHorizontalBox::Slot().VAlign(VAlign_Center)
							[
								SNew(SSearchBox)
								.HintText(LOCTEXT("SearchNodesHint", "Search Tracks"))
								.OnTextChanged(this, &SActActionSequenceWidget::OnOutlinerSearchChanged)
							]

						]
					]


					// main sequence area
					+ SGridPanel::Slot(0, 2, SGridPanel::Layer(10))
					.ColumnSpan(2)
					[
						SAssignNew(MainSequenceArea, SVerticalBox)

						+ SVerticalBox::Slot()
						  .AutoHeight()
						  .MaxHeight(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceWidget::GetPinnedAreaMaxHeight)))
						[
							SNew(SBorder)
							.Padding(FMargin(0.0f, 0.0f, 0.0f, 3.0f))
							[

								SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								[
									SNew(SOverlay)

									+ SOverlay::Slot()
									[
										SNew(SScrollBorder, ActActionSequenceTreeViewNode->GetTreeView())
										[
											SNew(SHorizontalBox)

											// outliner tree
											+ SHorizontalBox::Slot()
											.FillWidth(FillCoefficient_0)
											[
												SNew(SBox)
												[
													ActActionSequenceTreeViewNode->GetTreeViewPinned()
												]
											]

											// track area
											+ SHorizontalBox::Slot()
											.FillWidth(FillCoefficient_1)
											[
												SNew(SBox)
												.Padding(ResizeBarPadding)
												.Clipping(EWidgetClipping::ClipToBounds)
												[
													ActActionSequenceTreeViewNode->GetTrackAreaPinned()
												]
											]
										]
									]

									+ SOverlay::Slot()
									.HAlign(HAlign_Right)
									[
										PinnedAreaScrollBar
									]
								]
							]
						]

						+ SVerticalBox::Slot()
						[
							SNew(SHorizontalBox)

							+ SHorizontalBox::Slot()
							[
								SNew(SOverlay)

								+ SOverlay::Slot()
								[
									SNew(SScrollBorder, ActActionSequenceTreeViewNode->GetTreeView())
									[
										SNew(SHorizontalBox)

										// outliner tree
										+ SHorizontalBox::Slot()
										.FillWidth(FillCoefficient_0)
										[
											SNew(SBox)
											[
												ActActionSequenceTreeViewNode->GetTreeView()
											]
										]

										// track area
										+ SHorizontalBox::Slot()
										.FillWidth(FillCoefficient_1)
										[
											SNew(SBox)
											.Padding(ResizeBarPadding)
											.Clipping(EWidgetClipping::ClipToBounds)
											[
												ActActionSequenceTreeViewNode->GetTrackArea()
											]
										]
									]
								]

								+ SOverlay::Slot()
								.HAlign(HAlign_Right)
								[
									ScrollBar
								]
							]
						]
					]

					// Second column

					+ SGridPanel::Slot(1, 1)
					  .Padding(ResizeBarPadding)
					  .RowSpan(3)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSpacer)
						]
					]

					+ SGridPanel::Slot(1, 1, SGridPanel::Layer(10))
					.Padding(ResizeBarPadding)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.BorderBackgroundColor(FLinearColor(.50f, .50f, .50f, 1.0f))
						.Padding(0)
						.Clipping(EWidgetClipping::ClipToBounds)
						[
							GetActActionTimeSliderController()->GetActActionTimeSliderWidget()
						]
					]

					// Overlay that draws the tick lines
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(10))
					.Padding(ResizeBarPadding)
					[
						InSequenceController->GetActActionSequenceSectionOverlayController0()->GetActActionSequenceSectionOverlayWidget()
					]

					// Overlay that draws the scrub position
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(20))
					.Padding(ResizeBarPadding)
					[
						InSequenceController->GetActActionSequenceSectionOverlayController1()->GetActActionSequenceSectionOverlayWidget()
					]
				]
			]
		]
	];
}

// ** TODO:过滤子节点
void SActActionSequenceWidget::OnOutlinerSearchChanged(const FText& Filter)
{
}

float SActActionSequenceWidget::GetPinnedAreaMaxHeight() const
{
	if (!MainSequenceArea.IsValid())
	{
		return 0.0f;
	}

	// Allow the pinned area to use up to 2/3rds of the sequencer area
	return MainSequenceArea->GetCachedGeometry().GetLocalSize().Y * 0.666f;
}

TSharedRef<SWidget> SActActionSequenceWidget::MakeAddMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	ActActionSequenceController.Pin()->PopulateAddMenuContext(MenuBuilder);
	return MenuBuilder.MakeWidget();
}


TSharedRef<FActActionTimeSliderController> SActActionSequenceWidget::GetActActionTimeSliderController() const
{
	return ActActionSequenceController.Pin()->GetActActionTimeSliderController();
}

#undef LOCTEXT_NAMESPACE
