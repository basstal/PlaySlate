#include "ActActionSequenceWidget.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTreeView.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/TimeSlider/ActActionSequenceTimeSliderWidget.h"
#include "NovaSequenceEditor/Widgets/TimeSlider/ActActionSequenceSectionOverlay.h"

#include "FrameNumberNumericInterface.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "SEditorHeaderButton.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

SActActionSequenceWidget::~SActActionSequenceWidget()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceWidget::~SActActionSequenceWidget"));
}

void SActActionSequenceWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController)
{
	ColumnFillCoefficients[0] = 0.4f;
	ColumnFillCoefficients[1] = 0.6f;
	SequenceController = InSequenceController;

	TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	SAssignNew(TrackArea, SActActionSequenceTrackArea);
	SAssignNew(TreeView, SActActionSequenceTreeView, InSequenceController->GetTreeViewRoot(), TrackArea.ToSharedRef())
		.ExternalScrollbar(ScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds)
		.OnGetContextMenuContent(ActActionSequence::OnGetContextMenuContentDelegate::CreateSP(this, &SActActionSequenceWidget::PopulateAddMenuContext));

	TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	TSharedRef<SActActionSequenceTrackArea> PinnedTrackArea = SNew(SActActionSequenceTrackArea);
	TSharedRef<SActActionSequenceTreeView> PinnedTreeView = SNew(SActActionSequenceTreeView, InSequenceController->GetTreeViewRoot(), PinnedTrackArea)
		.ExternalScrollbar(PinnedAreaScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds)
		.OnGetContextMenuContent(ActActionSequence::OnGetContextMenuContentDelegate::CreateSP(this, &SActActionSequenceWidget::PopulateAddMenuContext));
	
	TSharedRef<FActActionSequenceEditor> ActActionSequenceEditor = InSequenceController->GetActActionSequenceEditor();
	// ** 初始化TimeSlider
	ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs;
	TimeSliderArgs.PlaybackRange = InArgs._PlaybackRange;
	TimeSliderArgs.DisplayRate = TAttribute<FFrameRate>(ActActionSequenceEditor, &FActActionSequenceEditor::GetDisplayRate);
	TimeSliderArgs.TickResolution = TAttribute<FFrameRate>(ActActionSequenceEditor, &FActActionSequenceEditor::GetTickResolution);
	TimeSliderArgs.SelectionRange = InArgs._SelectionRange;
	TimeSliderArgs.OnPlaybackRangeChanged = InArgs._OnPlaybackRangeChanged;
	TimeSliderArgs.ScrubPosition = InArgs._ScrubPosition;
	TimeSliderArgs.ScrubPositionText = InArgs._ScrubPositionText;
	TimeSliderArgs.OnBeginScrubberMovement = InArgs._OnBeginScrubbing;
	TimeSliderArgs.OnEndScrubberMovement = InArgs._OnEndScrubbing;
	TimeSliderArgs.OnScrubPositionChanged = InArgs._OnScrubPositionChanged;
	TimeSliderArgs.PlaybackStatus = InArgs._PlaybackStatus;
	TimeSliderArgs.NumericTypeInterface = InSequenceController->GetNumericType();
	TSharedPtr<FActActionTimeSliderController> TimeSliderControllerPtr = MakeShareable(new FActActionTimeSliderController(TimeSliderArgs, InSequenceController));
	TimeSliderControllerPtr->MakeTimeSliderWidget();
	SequenceTimeSliderWidget = TimeSliderControllerPtr->GetSequenceTimeSliderWidget();

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
									return SequenceController.Pin().IsValid();
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
										SNew(SScrollBorder, TreeView.ToSharedRef())
										[
											SNew(SHorizontalBox)

											// outliner tree
											+ SHorizontalBox::Slot()
											.FillWidth(FillCoefficient_0)
											[
												SNew(SBox)
												[
													PinnedTreeView
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
													PinnedTrackArea
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
									SNew(SScrollBorder, TreeView.ToSharedRef())
									[
										SNew(SHorizontalBox)

										// outliner tree
										+ SHorizontalBox::Slot()
										.FillWidth(FillCoefficient_0)
										[
											SNew(SBox)
											[
												TreeView.ToSharedRef()
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
												TrackArea.ToSharedRef()
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
							SequenceTimeSliderWidget.ToSharedRef()
						]
					]

					// Overlay that draws the tick lines
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(10))
					.Padding(ResizeBarPadding)
					[
						SNew(SActActionSequenceSectionOverlay, GetTimeSliderController())
						.Visibility(EVisibility::HitTestInvisible)
						.DisplayScrubPosition(false)
						.DisplayTickLines(true)
						.Clipping(EWidgetClipping::ClipToBounds)
					]

					// Overlay that draws the scrub position
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(20))
					.Padding(ResizeBarPadding)
					[
						SNew(SActActionSequenceSectionOverlay, GetTimeSliderController())
						.Visibility(EVisibility::HitTestInvisible)
						.DisplayScrubPosition(true)
						.DisplayTickLines(false)
						.DisplayMarkedFrames(true)
						.Clipping(EWidgetClipping::ClipToBounds)
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
	PopulateAddMenuContext(MenuBuilder);
	return MenuBuilder.MakeWidget();
}

void SActActionSequenceWidget::PopulateAddMenuContext(FMenuBuilder& MenuBuilder)
{
	if (!SequenceController.IsValid())
	{
		return;
	}
	// ** 填充AddTrack菜单
	TSharedRef<FActActionSequenceController> ActActionSequenceControllerRef = SequenceController.Pin().ToSharedRef();
	MenuBuilder.BeginSection("AddTracks");
	ActActionSequenceControllerRef->BuildAddTrackMenu(MenuBuilder);
	MenuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
