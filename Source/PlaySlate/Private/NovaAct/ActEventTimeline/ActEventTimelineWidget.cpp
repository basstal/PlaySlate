#include "ActEventTimelineWidget.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"

#include "NovaAct/ActEventTimeline/Slider/ActSliderWidget.h"
#include "NovaAct/ActEventTimeline/Image/ActImageHorizontalBox.h"
#include "NovaAct/ActViewport/ActViewport.h"
#include "NovaAct/ActEventTimeline/Components/ActTransportControlsWidget.h"
#include "NovaAct/ActEventTimeline/Components/ActSplitterOverlay.h"

#include "SEditorHeaderButton.h"
#include "FrameNumberNumericInterface.h"
#include "Image/ActImageScrubPosition.h"
#include "NovaAct/Assets/Tracks/ActActionHitBoxTrack.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"

#define LOCTEXT_NAMESPACE "NovaAct"

SActEventTimelineWidget::~SActEventTimelineWidget()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActEventTimelineWidget::~SActEventTimelineWidget"));
	NovaDB::Delete("ColumnFillCoefficientsLeft");
	TrackEditors.Empty();
	TrackEditorDelegates.Empty();
}

void SActEventTimelineWidget::Construct(const FArguments& InArgs)
{
	// ** TODO:存储在配置中
	ColumnFillCoefficientsLeftDB = NovaDB::Create("ColumnFillCoefficientsLeft", 0.3f);
	// ** 将能编辑的所有TrackEditor注册，以便能够使用AddTrackEditor以及AddTrackMenu
	TrackEditorDelegates.Add(OnCreateTrackEditorDelegate::CreateStatic(FActActionHitBoxTrack::CreateTrackEditor));

	auto FillLeftAttr = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([]()
	{
		auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
		return DB->GetData();
	}));
	auto FillRightAttr = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([]()
	{
		auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
		return 1 - DB->GetData();
	}));

	TSharedPtr<SGridPanel> GridPanel;

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
					// ** 第0列，第0行
					SAssignNew(GridPanel, SGridPanel)
					.FillRow(1, 1.0f)
					.FillColumn(0, FillLeftAttr)
					.FillColumn(1, FillRightAttr)
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
								.OnGetMenuContent(this, &SActEventTimelineWidget::BuildAddTrackMenuWidget)
								.Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
								.Text(LOCTEXT("Track", "Track"))
							]

							+ SHorizontalBox::Slot()
							.VAlign(VAlign_Center)
							[
								SNew(SSearchBox)
								.HintText(LOCTEXT("SearchNodesHint", "Search Tracks"))
								.OnTextChanged(this, &SActEventTimelineWidget::OnOutlinerSearchChanged)
							]
						]
					]


					// ** 第0列，第1行，整个 Widget 主要的区域，中间行，显示所有的 Event 以及它们的 Track - Notify
					+ SGridPanel::Slot(0, 1, SGridPanel::Layer(10))
					.ColumnSpan(2)
					[
						SNew(SActImageHorizontalBox)
					]

					+ SGridPanel::Slot(0, 2, SGridPanel::Layer(10))
					  .VAlign(VAlign_Center)
					  .HAlign(HAlign_Center)
					[
						SNew(SActTransportControlsWidget)
					]

					// ** 第1列，第0行
					+ SGridPanel::Slot(1, 0)
					  .Padding(NovaConst::ResizeBarPadding)
					  .RowSpan(2)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSpacer)
						]
					]

					// ** 第1列，第1行，Overlay that draws the tick lines
					+ SGridPanel::Slot(1, 1, SGridPanel::Layer(10))
					.Padding(NovaConst::ResizeBarPadding)
					[
						SNew(SActImageScrubPosition)
					]

					// ** 第1列，第1行，Overlay that draws the scrub position
					+ SGridPanel::Slot(1, 1, SGridPanel::Layer(20))
					.Padding(NovaConst::ResizeBarPadding)
					[
						SNew(SActImageThickLine)
					]
				]

				+ SOverlay::Slot()
				[
					// 绘制Overlay用于拖拽决定Sequence左右两块区域的占比
					SNew(SActSplitterOverlay)
					.Style(FEditorStyle::Get(), "AnimTimeline.Outliner.Splitter")
					.Visibility(EVisibility::SelfHitTestInvisible)
					+ SSplitter::Slot()
					  .Value(FillLeftAttr)
					  .OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([](float NewSizeCoefficient)
					  {
						  auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
						  DB->SetData(NewSizeCoefficient);
					  }))
					[
						SNew(SSpacer)
					]

					+ SSplitter::Slot()
					  .Value(FillRightAttr)
					  .OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([](float NewSizeCoefficient)
					  {
						  auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
						  DB->SetData(1 - NewSizeCoefficient);
					  }))
					[
						SNew(SSpacer)
					]
				]
			]
		]
	];


	if (GridPanel)
	{
		ActEventTimelineSliderWidget = SNew(SActSliderWidget, GridPanel.ToSharedRef());
	}

	// 调用已注册的TrackEditor的Create代理，并收集创建的TrackEditor实例
	for (int32 DelegateIndex = 0; DelegateIndex < TrackEditorDelegates.Num(); ++DelegateIndex)
	{
		check(TrackEditorDelegates[DelegateIndex].IsBound());
		// Tools may exist in other modules, call a delegate that will create one for us 
		TSharedRef<FActActionTrackEditorBase> TrackEditor = TrackEditorDelegates[DelegateIndex].Execute(SharedThis(this));
		TrackEditors.Add(TrackEditor);
	}
}

void SActEventTimelineWidget::OnOutlinerSearchChanged(const FText& Filter)
{
	InFilter = Filter;
}

TSharedRef<SWidget> SActEventTimelineWidget::BuildAddTrackMenuWidget()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	// ** 填充AddTrack菜单
	MenuBuilder.BeginSection("AddTracks");
	for (int32 i = 0; i < TrackEditors.Num(); ++i)
	{
		TrackEditors[i]->BuildAddTrackMenu(MenuBuilder);
	}
	MenuBuilder.EndSection();
	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
