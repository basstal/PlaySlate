#include "ActActionSequenceWidget.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTreeView.h"
#include "NovaSequenceEditor/Widgets/TimeSlider/ActActionSequenceTimeSliderWidget.h"
#include "NovaSequenceEditor/Widgets/TimeSlider/ActActionSequenceSectionOverlay.h"

#include "FrameNumberNumericInterface.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "SEditorHeaderButton.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

SActActionSequenceWidget::SActActionSequenceWidget()
{
	ColumnFillCoefficients[0] = 0.4f;
	ColumnFillCoefficients[1] = 0.6f;
}

SActActionSequenceWidget::~SActActionSequenceWidget()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceWidget::~SActActionSequenceWidget"));

	// SequenceController.Reset();
}

void SActActionSequenceWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController)
{
	SequenceController = InSequenceController;

	TAttribute<float> FillCoefficient_0, FillCoefficient_1;
	FillCoefficient_0.Bind(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceWidget::GetColumnFillCoefficient, 0));
	FillCoefficient_1.Bind(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceWidget::GetColumnFillCoefficient, 1));

	TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));

	SAssignNew(TrackArea, SActActionSequenceTrackArea);
	SAssignNew(TreeView, SActActionSequenceTreeView, InSequenceController->GetNodeTree(), TrackArea.ToSharedRef())
		.ExternalScrollbar(ScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds)
		.OnGetContextMenuContent(ActActionSequence::OnGetContextMenuContentDelegate::CreateSP(this, &SActActionSequenceWidget::PopulateAddMenuContext));

	SAssignNew(PinnedTrackArea, SActActionSequenceTrackArea);
	SAssignNew(PinnedTreeView, SActActionSequenceTreeView, InSequenceController->GetNodeTree(), PinnedTrackArea.ToSharedRef())
		.ExternalScrollbar(PinnedAreaScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds)
		.OnGetContextMenuContent(ActActionSequence::OnGetContextMenuContentDelegate::CreateSP(this, &SActActionSequenceWidget::PopulateAddMenuContext));

	// Get the desired display format from the user's settings each time.
	TAttribute<EFrameNumberDisplayFormats> GetDisplayFormatAttr = MakeAttributeLambda([=]
	{
		// if (USequencerSettings* Settings = GetSequencerSettings())
		// {
		// 	return Settings->GetTimeDisplayFormat();
		// }
		return EFrameNumberDisplayFormats::Frames;
	});
	// Get the number of zero pad frames from the user's settings as well.
	TAttribute<uint8> GetZeroPadFramesAttr = MakeAttributeLambda([=]()-> uint8
	{
		// if (USequencerSettings* Settings = GetSequencerSettings())
		// {
		// 	return Settings->GetZeroPadFrames();
		// }
		return 0;
	});
	TAttribute<FFrameRate> GetTickResolutionAttr = TAttribute<FFrameRate>(InSequenceController, &FActActionSequenceController::GetFocusedTickResolution);
	TAttribute<FFrameRate> GetDisplayRateAttr = TAttribute<FFrameRate>(InSequenceController, &FActActionSequenceController::GetFocusedDisplayRate);
	// Create our numeric type interface so we can pass it to the time slider below.
	NumericTypeInterface = MakeShareable(new FFrameNumberInterface(GetDisplayFormatAttr, GetZeroPadFramesAttr, GetTickResolutionAttr, GetDisplayRateAttr));

	// ** 初始化TimeSlider
	ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs;
	TimeSliderArgs.ViewRange = InArgs._ViewRange;
	TimeSliderArgs.ClampRange = InArgs._ClampRange;
	TimeSliderArgs.PlaybackRange = InArgs._PlaybackRange;
	TimeSliderArgs.DisplayRate = TAttribute<FFrameRate>(InSequenceController, &FActActionSequenceController::GetFocusedDisplayRate);
	TimeSliderArgs.TickResolution = TAttribute<FFrameRate>(InSequenceController, &FActActionSequenceController::GetFocusedTickResolution);
	TimeSliderArgs.SelectionRange = InArgs._SelectionRange;
	TimeSliderArgs.OnPlaybackRangeChanged = InArgs._OnPlaybackRangeChanged;
	TimeSliderArgs.OnPlaybackRangeBeginDrag = InArgs._OnPlaybackRangeBeginDrag;
	TimeSliderArgs.OnPlaybackRangeEndDrag = InArgs._OnPlaybackRangeEndDrag;
	TimeSliderArgs.OnSelectionRangeChanged = InArgs._OnSelectionRangeChanged;
	TimeSliderArgs.OnSelectionRangeBeginDrag = InArgs._OnSelectionRangeBeginDrag;
	TimeSliderArgs.OnSelectionRangeEndDrag = InArgs._OnSelectionRangeEndDrag;
	TimeSliderArgs.OnMarkBeginDrag = InArgs._OnMarkBeginDrag;
	TimeSliderArgs.OnMarkEndDrag = InArgs._OnMarkEndDrag;
	TimeSliderArgs.OnViewRangeChanged = InArgs._OnViewRangeChanged;
	TimeSliderArgs.OnClampRangeChanged = InArgs._OnClampRangeChanged;
	TimeSliderArgs.OnGetNearestKey = InArgs._OnGetNearestKey;
	TimeSliderArgs.IsPlaybackRangeLocked = InArgs._IsPlaybackRangeLocked;
	TimeSliderArgs.OnTogglePlaybackRangeLocked = InArgs._OnTogglePlaybackRangeLocked;
	TimeSliderArgs.ScrubPosition = InArgs._ScrubPosition;
	TimeSliderArgs.ScrubPositionText = InArgs._ScrubPositionText;
	// TimeSliderArgs.ScrubPositionParent = InArgs._ScrubPositionParent;
	// TimeSliderArgs.ScrubPositionParentChain = InArgs._ScrubPositionParentChain;
	// TimeSliderArgs.OnScrubPositionParentChanged = InArgs._OnScrubPositionParentChanged;
	TimeSliderArgs.OnBeginScrubberMovement = InArgs._OnBeginScrubbing;
	TimeSliderArgs.OnEndScrubberMovement = InArgs._OnEndScrubbing;
	TimeSliderArgs.OnScrubPositionChanged = InArgs._OnScrubPositionChanged;
	TimeSliderArgs.PlaybackStatus = InArgs._PlaybackStatus;
	TimeSliderArgs.SubSequenceRange = InArgs._SubSequenceRange;
	TimeSliderArgs.VerticalFrames = InArgs._VerticalFrames;
	// TimeSliderArgs.MarkedFrames = InArgs._MarkedFrames;
	// TimeSliderArgs.GlobalMarkedFrames = InArgs._GlobalMarkedFrames;
	TimeSliderArgs.OnSetMarkedFrame = InArgs._OnSetMarkedFrame;
	TimeSliderArgs.OnAddMarkedFrame = InArgs._OnAddMarkedFrame;
	TimeSliderArgs.OnDeleteMarkedFrame = InArgs._OnDeleteMarkedFrame;
	TimeSliderArgs.OnDeleteAllMarkedFrames = InArgs._OnDeleteAllMarkedFrames;
	TimeSliderArgs.NumericTypeInterface = NumericTypeInterface;
	
	TimeSliderController = MakeShareable(new FActActionTimeSliderController(TimeSliderArgs, InSequenceController));
	// Create the top and bottom sliders
	TopTimeSlider = SNew(SActActionSequenceTimeSliderWidget, TimeSliderController.ToSharedRef())
		.MirrorLabels(false);

	const FMargin ResizeBarPadding(4.0f, 0, 0, 0);

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

					// ** FIX:outliner search box 搜索框？
					+ SGridPanel::Slot(0, 1, SGridPanel::Layer(10))
					[
						SNew(SBorder).BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(0.6f, 0.3f)).Clipping(EWidgetClipping::ClipToBounds)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FMargin(0, 0, 0.3f, 0))
							[
								MakeAddButton()
							]

							+ SHorizontalBox::Slot().VAlign(VAlign_Center)
							[
								SAssignNew(SearchBox, SSearchBox).HintText(LOCTEXT("SearchNodesHint", "Search Tracks")).OnTextChanged(this, &SActActionSequenceWidget::OnOutlinerSearchChanged)
							]

						]
					]


					// main sequencer area
					+ SGridPanel::Slot(0, 2, SGridPanel::Layer(10))
					.ColumnSpan(2)
					[
						SAssignNew(MainSequenceArea, SVerticalBox)

						+ SVerticalBox::Slot()
						  .AutoHeight()
						  .MaxHeight(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceWidget::GetPinnedAreaMaxHeight)))
						[
							SNew(SBorder)
							.Visibility(this, &SActActionSequenceWidget::GetPinnedAreaVisibility)
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
													PinnedTreeView.ToSharedRef()
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
													PinnedTrackArea.ToSharedRef()
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

								// + SOverlay::Slot()
								// .VAlign(VAlign_Bottom)
								// [
								// 	SAssignNew(SequencerTreeFilterStatusBar, SSequencerTreeFilterStatusBar, InSequencer)
								// 	.Visibility(EVisibility::SelfHitTestInvisible)
								// ]
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
							TopTimeSlider.ToSharedRef()
						]
					]

					// Overlay that draws the tick lines
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(10))
					.Padding(ResizeBarPadding)
					[
						SNew(SActActionSequenceSectionOverlay, TimeSliderController.ToSharedRef())
						.Visibility(EVisibility::HitTestInvisible)
						.DisplayScrubPosition(false)
						.DisplayTickLines(true)
						.Clipping(EWidgetClipping::ClipToBounds)
					]

					// Overlay that draws the scrub position
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(20))
					.Padding(ResizeBarPadding)
					[
						SNew(SActActionSequenceSectionOverlay, TimeSliderController.ToSharedRef())
						.Visibility(EVisibility::HitTestInvisible)
						.DisplayScrubPosition(true)
						.DisplayTickLines(false)
						.DisplayMarkedFrames(true)
						.PaintPlaybackRangeArgs(this, &SActActionSequenceWidget::GetSectionPlaybackRangeArgs)
						.Clipping(EWidgetClipping::ClipToBounds)
					]

					// + SGridPanel::Slot(1, 2, SGridPanel::Layer(30))
					//   .Padding(ResizeBarPadding)
					//   .HAlign(HAlign_Left)
					//   .VAlign(VAlign_Top)
					// [
					// 	// Transform box
					// 	SAssignNew(TransformBox, SSequencerTransformBox, SequencerPtr.Pin().ToSharedRef(), *GetSequencerSettings(), NumericTypeInterface.ToSharedRef())
					// ]

					// + SGridPanel::Slot(1, 2, SGridPanel::Layer(40))
					// .Padding(ResizeBarPadding)
					// [
					// 	SAssignNew(TickResolutionOverlay, SSequencerTimePanel, SequencerPtr)
					// ]

					// + SGridPanel::Slot(1, 2, SGridPanel::Layer(50))
					//   .Padding(ResizeBarPadding)
					//   .HAlign(HAlign_Left)
					//   .VAlign(VAlign_Top)
					// [
					// 	// Stretch box
					// 	SAssignNew(StretchBox, SSequencerStretchBox, SequencerPtr.Pin().ToSharedRef(), *GetSequencerSettings(), NumericTypeInterface.ToSharedRef())
					// ]
				]

			]
		]
	];
}

int32 SActActionSequenceWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void SActActionSequenceWidget::OnOutlinerSearchChanged(const FText& Filter)
{
	TSharedPtr<FActActionSequenceController> Sequence = SequenceController.Pin();
	if (Sequence.IsValid())
	{
		const FString FilterString = Filter.ToString();
		// Sequence->GetNodeTree()->SetFilterNodes(FilterString);
		// TreeView->Refresh();
	}
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

EVisibility SActActionSequenceWidget::GetPinnedAreaVisibility() const
{
	// return PinnedTreeView->GetNumRootNodes() > 0 ? EVisibility::Visible : EVisibility::Collapsed;
	return EVisibility::Visible;
}

TSharedRef<SWidget> SActActionSequenceWidget::MakeAddButton()
{
	return SNew(SEditorHeaderButton)
	.OnGetMenuContent(this, &SActActionSequenceWidget::MakeAddMenu)
	.Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
	.Text(LOCTEXT("Track", "Track"))
	.IsEnabled_Lambda([=]()
	{
		return SequenceController.Pin().IsValid();
	});
}

TSharedRef<SWidget> SActActionSequenceWidget::MakeAddMenu()
{
	TSharedPtr<FExtender> Extender = FExtender::Combine(AddMenuExtenders);
	FMenuBuilder MenuBuilder(true, nullptr, Extender);
	PopulateAddMenuContext(MenuBuilder);
	return MenuBuilder.MakeWidget();
}

void SActActionSequenceWidget::PopulateAddMenuContext(FMenuBuilder& MenuBuilder)
{
	// ** 让Toolkits填充该菜单
	TSharedPtr<FActActionSequenceController> Sequence = SequenceController.Pin();
	MenuBuilder.BeginSection("MainMenu");
	if (Sequence.IsValid())
	{
		OnGetAddMenuContent.ExecuteIfBound(MenuBuilder, Sequence.ToSharedRef());
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("ObjectBindings");
	if (Sequence.IsValid())
	{
		Sequence->BuildAddObjectBindingsMenu(MenuBuilder);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("AddTracks");
	if (Sequence.IsValid())
	{
		Sequence->BuildAddTrackMenu(MenuBuilder);
	}
	MenuBuilder.EndSection();
}

ActActionSequence::FActActionPaintPlaybackRangeArgs SActActionSequenceWidget::GetSectionPlaybackRangeArgs() const
{
	// if (GetBottomTimeSliderVisibility() == EVisibility::Visible)
	// {
	static ActActionSequence::FActActionPaintPlaybackRangeArgs Args
	{
		FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_L"),
		FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_R"),
		6.f,
	};
	return Args;
	// }
	// else
	// {
	// 	static FPaintPlaybackRangeArgs Args(FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_Bottom_L"), FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_Bottom_R"), 6.f);
	// 	return Args;
	// }
}
#undef LOCTEXT_NAMESPACE
