#include "ActActionSequenceWidget.h"

#include "PlaySlate.h"
#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineBrain.h"
#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineSlider.h"
#include "NovaAct/Widgets/ActViewport/ActActionViewportWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/ActActionSequenceTrackArea.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/ActActionSequenceTreeView.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActEventTimeline/Image/ActActionTimeSliderWidget.h"
#include "NovaAct/Widgets/ActEventTimeline/Image/ActActionSequenceSectionOverlayWidget.h"
#include "NovaAct/Widgets/ActEventTimeline/Subs/ActActionSequenceTransportControls.h"
#include "NovaAct/Widgets/ActEventTimeline/Subs/ActActionSequenceSplitterOverlay.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActEventTimeline/Image/Subs/ActActionTimeRange.h"

#include "SEditorHeaderButton.h"
#include "FrameNumberNumericInterface.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "NovaAct/Controllers/ActEventTimeline/SequenceNodeTree/ActActionSequenceTreeViewNode.h"

#define LOCTEXT_NAMESPACE "NovaAct"

SActActionSequenceWidget::SActActionSequenceWidget()
	: ColumnFillCoefficients{0.3, 0.7}
{
}

SActActionSequenceWidget::~SActActionSequenceWidget()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceWidget::~SActActionSequenceWidget"));
}

void SActActionSequenceWidget::Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineBrain>& InActActionSequenceController)
{
	ColumnFillCoefficients[0] = 0.4f;
	ColumnFillCoefficients[1] = 0.6f;
	ActActionSequenceController = InActActionSequenceController;

	const TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	const TSharedRef<FActActionSequenceTreeViewNode> ActActionSequenceTreeViewNode = InActActionSequenceController->GetActActionSequenceTreeViewRoot();
	ActActionSequenceTreeViewNode->MakeActActionSequenceTreeView(ScrollBar);
	const TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
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
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			+ SSplitter::Slot()
			.Value(0.9f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SGridPanel)
					.FillRow(1, 1.0f)
					.FillColumn(0, FillCoefficient_0)
					.FillColumn(1, FillCoefficient_1)
					// ** TODO:暂缺工具栏

					// ** 第0列，第0行
					+ SGridPanel::Slot(0, 0)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSpacer)
						]
					]

					// ** 第0列，第0行
					+ SGridPanel::Slot(0, 0, SGridPanel::Layer(10))
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.Padding(FMargin(0.6f, 0.3f))
						.Clipping(EWidgetClipping::ClipToBounds)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							  .AutoWidth()
							  .VAlign(VAlign_Center)
							  .Padding(FMargin(0, 0, 0.3f, 0))
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

							+ SHorizontalBox::Slot()
							.VAlign(VAlign_Center)
							[
								SNew(SSearchBox)
								.HintText(LOCTEXT("SearchNodesHint", "Search Tracks"))
								.OnTextChanged(this, &SActActionSequenceWidget::OnOutlinerSearchChanged)
							]
						]
					]


					// ** 第0列，第1行，main sequence area
					+ SGridPanel::Slot(0, 1, SGridPanel::Layer(10))
					.ColumnSpan(2)
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

					+ SGridPanel::Slot(0, 2, SGridPanel::Layer(10))
					  .VAlign(VAlign_Center)
					  .HAlign(HAlign_Center)
					[
						SNew(SActActionSequenceTransportControls, InActActionSequenceController)
					]

					// ** 第1列，第0行
					+ SGridPanel::Slot(1, 0)
					  .Padding(ResizeBarPadding)
					  .RowSpan(2)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSpacer)
						]
					]

					// ** 第1列，第0行
					+ SGridPanel::Slot(1, 0, SGridPanel::Layer(10))
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

					// ** 第1列，第1行，Overlay that draws the tick lines
					+ SGridPanel::Slot(1, 1, SGridPanel::Layer(10))
					.Padding(ResizeBarPadding)
					[
						GetActActionTimeSliderController()->GetScrubPosSequenceSectionOverlayController()->GetActActionSequenceSectionOverlayWidget()
					]

					// ** 第1列，第1行，Overlay that draws the scrub position
					+ SGridPanel::Slot(1, 1, SGridPanel::Layer(20))
					.Padding(ResizeBarPadding)
					[
						GetActActionTimeSliderController()->GetTickLinesSequenceSectionOverlayController()->GetActActionSequenceSectionOverlayWidget()
					]
					// play range slider
					+ SGridPanel::Slot(1, 2, SGridPanel::Layer(10))
					.Padding(ResizeBarPadding)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
						.Clipping(EWidgetClipping::ClipToBounds)
						.Padding(0)
						[
							InActActionSequenceController->GetActActionTimeSliderController()->GetActActionTimeRange()
						]
					]
				]
				+ SOverlay::Slot()
				[
					// 绘制Overlay用于拖拽决定Sequence左右两块区域的占比
					SNew(SActActionSequenceSplitterOverlay)
					.Style(FEditorStyle::Get(), "AnimTimeline.Outliner.Splitter")
					.Visibility(EVisibility::SelfHitTestInvisible)

					+ SSplitter::Slot()
					  .Value(FillCoefficient_0)
					  .OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([this](float NewSizeCoefficient)
					  {
						  ColumnFillCoefficients[0] = NewSizeCoefficient;
					  }))
					[
						SNew(SSpacer)
					]

					+ SSplitter::Slot()
					  .Value(FillCoefficient_1)
					  .OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([this](float NewSizeCoefficient)
					  {
						  ColumnFillCoefficients[1] = NewSizeCoefficient;
					  }))
					[
						SNew(SSpacer)
					]
				]
			]
		]
	];
}

// ** TODO:过滤子节点
void SActActionSequenceWidget::OnOutlinerSearchChanged(const FText& Filter)
{
	InFilter = Filter;
}

TSharedRef<SWidget> SActActionSequenceWidget::MakeAddMenu() const
{
	FMenuBuilder MenuBuilder(true, nullptr);
	ActActionSequenceController.Pin()->PopulateAddMenuContext(MenuBuilder);
	return MenuBuilder.MakeWidget();
}


TSharedRef<FActEventTimelineSlider> SActActionSequenceWidget::GetActActionTimeSliderController() const
{
	return ActActionSequenceController.Pin()->GetActActionTimeSliderController();
}

#undef LOCTEXT_NAMESPACE
