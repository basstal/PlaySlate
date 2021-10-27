#include "ActSliderViewRangeWidget.h"

#include "NovaAct/ActEventTimeline/Slider/ActSliderViewRangeBarWidget.h"
#include "NovaAct/ActEventTimeline/Slider/ActSliderWidget.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "NovaAct"

void SActSliderViewRangeWidget::Construct(const FArguments& InArgs)
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();

	ChildSlot
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
					SNew(SSpinBox<double>)
					.Value(this, &SActSliderViewRangeWidget::ViewStartTime)
					.ToolTipText(LOCTEXT("ViewStartTimeTooltip", "View Range Start Time"))
					.OnValueCommitted(this, &SActSliderViewRangeWidget::OnViewStartTimeCommitted)
					.OnValueChanged(SSpinBox<double>::FOnValueChanged::CreateLambda([this](double InNewValue)
					                      {
						                      OnViewTimeChanged(InNewValue);
					                      }))
					.MinValue(TOptional<double>())
					.MaxValue(TOptional<double>())
					.Style(&FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>("Sequencer.HyperlinkSpinBox"))
					.TypeInterface(ActEventTimelineArgs->NumericTypeInterface)
					.ClearKeyboardFocusOnCommit(true)
					.Delta(this, &SActSliderViewRangeWidget::GetSpinboxDelta)
					.LinearDeltaSensitivity(25)
				]
			]

			+ SHorizontalBox::Slot()
			  .FillWidth(1.0f)
			  .Padding(2.0f, 4.0f)
			  .VAlign(VAlign_Center)
			[
				SNew(SActSliderViewRangeBarWidget)
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
					SNew(SSpinBox<double>)
					.Value(this, &SActSliderViewRangeWidget::ViewEndTime)
					.ToolTipText(LOCTEXT("ViewEndTimeTooltip", "View Range End Time"))
					.OnValueCommitted(this, &SActSliderViewRangeWidget::OnViewEndTimeCommitted)
					.OnValueChanged(SSpinBox<double>::FOnValueChanged::CreateLambda([this](double InNewValue)
					                      {
						                      OnViewTimeChanged(InNewValue, true);
					                      }))
					.MinValue(TOptional<double>())
					.MaxValue(TOptional<double>())
					.Style(&FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>("Sequencer.HyperlinkSpinBox"))
					.TypeInterface(ActEventTimelineArgs->NumericTypeInterface)
					.ClearKeyboardFocusOnCommit(true)
					.Delta(this, &SActSliderViewRangeWidget::GetSpinboxDelta)
					.LinearDeltaSensitivity(25)
				]
			]
		];
}


double SActSliderViewRangeWidget::ViewStartTime() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	// View range is in seconds so we convert it to tick resolution
	const FFrameTime Time = ActEventTimelineArgs->ViewRange->GetLowerBoundValue() * TickResolution;
	return Time.GetFrame().Value;
}

void SActSliderViewRangeWidget::OnViewStartTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const
{
	OnViewTimeChanged(NewValue);
}

void SActSliderViewRangeWidget::OnViewTimeChanged(double NewValue, bool bIsEndValue) const
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

double SActSliderViewRangeWidget::GetSpinboxDelta() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	return ActEventTimelineArgs->TickResolution.AsDecimal() * ActEventTimelineArgs->TickResolution.AsInterval();
}

double SActSliderViewRangeWidget::ViewEndTime() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	// View range is in seconds so we convert it to tick resolution
	const FFrameTime Time = ActEventTimelineArgs->ViewRange->GetUpperBoundValue() * TickResolution;
	return Time.GetFrame().Value;
}

void SActSliderViewRangeWidget::OnViewEndTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const
{
	OnViewTimeChanged(NewValue, true);
}
#undef LOCTEXT_NAMESPACE
