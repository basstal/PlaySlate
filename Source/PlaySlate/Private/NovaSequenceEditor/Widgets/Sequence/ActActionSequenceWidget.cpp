﻿#include "ActActionSequenceWidget.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/Sequence/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionSequenceTrackArea.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionSequenceTreeView.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/ActActionTimeSliderWidget.h"
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/ActActionSequenceSectionOverlayWidget.h"
#include "NovaSequenceEditor/Widgets/Sequence/Subs/ActActionSequenceTransportControls.h"
#include "NovaSequenceEditor/Widgets/Sequence/Subs/ActActionSequenceSplitterOverlay.h"

#include "FrameNumberNumericInterface.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "SEditorHeaderButton.h"
#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

SActActionSequenceWidget::~SActActionSequenceWidget()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceWidget::~SActActionSequenceWidget"));
}

void SActActionSequenceWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InActActionSequenceController)
{
	ColumnFillCoefficients[0] = 0.4f;
	ColumnFillCoefficients[1] = 0.6f;
	ActActionSequenceController = InActActionSequenceController;

	TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	TSharedRef<FActActionSequenceTreeViewNode> ActActionSequenceTreeViewNode = InActActionSequenceController->GetActActionSequenceTreeViewRoot();
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
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			+ SSplitter::Slot()
			.Value(0.9f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SGridPanel)
					.FillRow(2, 1.0f)
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

					// ** 第0列，第2行
					+ SGridPanel::Slot(0, 3, SGridPanel::Layer(10))
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
						InActActionSequenceController->GetActActionSequenceSectionOverlayController0()->GetActActionSequenceSectionOverlayWidget()
					]

					// ** 第1列，第1行，Overlay that draws the scrub position
					+ SGridPanel::Slot(1, 1, SGridPanel::Layer(20))
					.Padding(ResizeBarPadding)
					[
						InActActionSequenceController->GetActActionSequenceSectionOverlayController1()->GetActActionSequenceSectionOverlayWidget()
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
