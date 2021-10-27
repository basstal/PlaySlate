#include "ActEventTimelineViewRangeCommitWidget.h"

#include "NovaAct/ActEventTimeline/Slider/ActEventTimelineViewRangeBarWidget.h"
#include "NovaAct/ActEventTimeline/Slider/ActEventTimelineSliderWidget.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "NovaAct"

void SActEventTimelineViewRangeCommitWidget::Construct(const FArguments& InArgs)
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const TSharedRef<SWidget> ViewRangeStart = SNew(SSpinBox<double>)
	.Value(this, &SActEventTimelineViewRangeCommitWidget::ViewStartTime)
	.ToolTipText(LOCTEXT("ViewStartTimeTooltip", "View Range Start Time"))
	.OnValueCommitted(this, &SActEventTimelineViewRangeCommitWidget::OnViewStartTimeCommitted)
	.OnValueChanged(SSpinBox<double>::FOnValueChanged::CreateLambda([this](double InNewValue)
	                                                                 {
		                                                                 OnViewTimeChanged(InNewValue);
	                                                                 }))
	.MinValue(TOptional<double>())
	.MaxValue(TOptional<double>())
	.Style(&FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>("Sequencer.HyperlinkSpinBox"))
	.TypeInterface(ActEventTimelineArgs->NumericTypeInterface)
	.ClearKeyboardFocusOnCommit(true)
	.Delta(this, &SActEventTimelineViewRangeCommitWidget::GetSpinboxDelta)
	.LinearDeltaSensitivity(25);


	const TSharedRef<SWidget> ViewRangeEnd = SNew(SSpinBox<double>)
	.Value(this, &SActEventTimelineViewRangeCommitWidget::ViewEndTime)
	.ToolTipText(LOCTEXT("ViewEndTimeTooltip", "View Range End Time"))
	.OnValueCommitted(this, &SActEventTimelineViewRangeCommitWidget::OnViewEndTimeCommitted)
	.OnValueChanged(SSpinBox<double>::FOnValueChanged::CreateLambda([this](double InNewValue)
	                                                               {
		                                                               OnViewTimeChanged(InNewValue, true);
	                                                               }))
	.MinValue(TOptional<double>())
	.MaxValue(TOptional<double>())
	.Style(&FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>("Sequencer.HyperlinkSpinBox"))
	.TypeInterface(ActEventTimelineArgs->NumericTypeInterface)
	.ClearKeyboardFocusOnCommit(true)
	.Delta(this, &SActEventTimelineViewRangeCommitWidget::GetSpinboxDelta)
	.LinearDeltaSensitivity(25);

	this->ChildSlot
	    .HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		  .VAlign(VAlign_Center)
		  .AutoWidth()
		  .Padding(2.f)
		[
			SNew(SBox)
			.MinDesiredWidth(64)
			.HAlign(HAlign_Center)
			[
				ViewRangeStart
			]
		]

		+ SHorizontalBox::Slot()
		  .FillWidth(1.0f)
		  .Padding(2.0f, 4.0f)
		  .VAlign(VAlign_Center)
		[
			SNew(SActEventTimelineViewRangeBarWidget)
		]

		+ SHorizontalBox::Slot()
		  .VAlign(VAlign_Center)
		  .AutoWidth()
		  .Padding(2.f)
		[
			SNew(SBox)
			.MinDesiredWidth(64)
			.HAlign(HAlign_Center)
			[
				ViewRangeEnd
			]
		]
	];
}


double SActEventTimelineViewRangeCommitWidget::ViewStartTime() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	// View range is in seconds so we convert it to tick resolution
	const FFrameTime Time = ActEventTimelineArgs->ViewRange->GetLowerBoundValue() * TickResolution;
	return Time.GetFrame().Value;
}

void SActEventTimelineViewRangeCommitWidget::OnViewStartTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const
{
	OnViewTimeChanged(NewValue);
}

void SActEventTimelineViewRangeCommitWidget::OnViewTimeChanged(double NewValue, bool bIsEndValue) const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	const double Time = TickResolution.AsSeconds(FFrameTime::FromDecimal(NewValue));
	const double ViewStartTime = ActEventTimelineArgs->ViewRange->GetLowerBoundValue();
	double ViewEndTime = ActEventTimelineArgs->ViewRange->GetUpperBoundValue();

	if (Time >= ViewEndTime)
	{
		const double ViewDuration = ViewEndTime - ViewStartTime;
		ViewEndTime = Time + ViewDuration;
	}
	if (bIsEndValue)
	{
		ActEventTimelineArgs->ViewRange->SetLowerBoundValue(ViewStartTime);
		ActEventTimelineArgs->ViewRange->SetUpperBoundValue(Time);
		NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
	}
	else
	{
		ActEventTimelineArgs->ViewRange->SetLowerBoundValue(Time);
		ActEventTimelineArgs->ViewRange->SetUpperBoundValue(ViewEndTime);
		NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
	}
}

double SActEventTimelineViewRangeCommitWidget::GetSpinboxDelta() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	return ActEventTimelineArgs->TickResolution.AsDecimal() * ActEventTimelineArgs->TickResolution.AsInterval();
}

double SActEventTimelineViewRangeCommitWidget::ViewEndTime() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	// View range is in seconds so we convert it to tick resolution
	const FFrameTime Time = ActEventTimelineArgs->ViewRange->GetUpperBoundValue() * TickResolution;
	return Time.GetFrame().Value;
}

void SActEventTimelineViewRangeCommitWidget::OnViewEndTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const
{
	OnViewTimeChanged(NewValue, true);
}
#undef LOCTEXT_NAMESPACE
