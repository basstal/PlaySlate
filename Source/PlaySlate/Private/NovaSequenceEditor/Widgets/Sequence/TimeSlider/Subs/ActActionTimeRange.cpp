#include "ActActionTimeRange.h"

#include "ActActionTimeRangeSlider.h"
#include "NovaSequenceEditor/Controllers/Sequence/TimeSlider/ActActionTimeSliderController.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "ActActionSequence"

void SActActionTimeRange::Construct(const FArguments& InArgs, const TSharedRef<FActActionTimeSliderController>& InTimeSliderController, const TSharedRef<SActActionTimeRangeSlider>& InTimeRangeSlider)
{
	TimeSliderController = InTimeSliderController;

	const TSharedRef<SWidget> ViewRangeStart = SNew(SSpinBox<double>)
	.Value(this, &SActActionTimeRange::ViewStartTime)
	.ToolTipText(LOCTEXT("ViewStartTimeTooltip", "View Range Start Time"))
	.OnValueCommitted(this, &SActActionTimeRange::OnViewStartTimeCommitted)
	.OnValueChanged(SSpinBox<double>::FOnValueChanged::CreateLambda([this](double InNewValue)
	                                                                 {
		                                                                 OnViewTimeChanged(InNewValue);
	                                                                 }))
	.MinValue(TOptional<double>())
	.MaxValue(TOptional<double>())
	.Style(&FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>("Sequencer.HyperlinkSpinBox"))
	.TypeInterface(InTimeSliderController->GetTimeSliderArgs().NumericTypeInterface)
	.ClearKeyboardFocusOnCommit(true)
	.Delta(this, &SActActionTimeRange::GetSpinboxDelta)
	.LinearDeltaSensitivity(25);


	const TSharedRef<SWidget> ViewRangeEnd = SNew(SSpinBox<double>)
	.Value(this, &SActActionTimeRange::ViewEndTime)
	.ToolTipText(LOCTEXT("ViewEndTimeTooltip", "View Range End Time"))
	.OnValueCommitted(this, &SActActionTimeRange::OnViewEndTimeCommitted)
	.OnValueChanged(SSpinBox<double>::FOnValueChanged::CreateLambda([this](double InNewValue)
	                                                               {
		                                                               OnViewTimeChanged(InNewValue, true);
	                                                               }))
	.MinValue(TOptional<double>())
	.MaxValue(TOptional<double>())
	.Style(&FEditorStyle::Get().GetWidgetStyle<FSpinBoxStyle>("Sequencer.HyperlinkSpinBox"))
	.TypeInterface(InTimeSliderController->GetTimeSliderArgs().NumericTypeInterface)
	.ClearKeyboardFocusOnCommit(true)
	.Delta(this, &SActActionTimeRange::GetSpinboxDelta)
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
			InTimeRangeSlider
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


double SActActionTimeRange::ViewStartTime() const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = TimeSliderController.Pin()->GetTimeSliderArgs();
	const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	// View range is in seconds so we convert it to tick resolution
	const FFrameTime Time = TimeSliderArgs.ViewRange.Get().GetLowerBoundValue() * TickResolution;
	return Time.GetFrame().Value;
}

void SActActionTimeRange::OnViewStartTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const
{
	OnViewTimeChanged(NewValue);
}

void SActActionTimeRange::OnViewTimeChanged(double NewValue, bool bIsEndValue) const
{
	if (!TimeSliderController.IsValid())
	{
		return;
	}
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = TimeSliderController.Pin()->GetTimeSliderArgs();
	const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	const double Time = TickResolution.AsSeconds(FFrameTime::FromDecimal(NewValue));
	const double ViewStartTime = TimeSliderArgs.ViewRange.Get().GetLowerBoundValue();
	double ViewEndTime = TimeSliderArgs.ViewRange.Get().GetUpperBoundValue();

	if (Time >= ViewEndTime)
	{
		const double ViewDuration = ViewEndTime - ViewStartTime;
		ViewEndTime = Time + ViewDuration;
	}
	if (bIsEndValue)
	{
		TimeSliderController.Pin()->SetViewRange(ViewStartTime, Time, ActActionSequence::EActActionViewRangeInterpolation::Immediate);
	}
	else
	{
		TimeSliderController.Pin()->SetViewRange(Time, ViewEndTime, ActActionSequence::EActActionViewRangeInterpolation::Immediate);
	}
}

double SActActionTimeRange::GetSpinboxDelta() const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = TimeSliderController.Pin()->GetTimeSliderArgs();
	return TimeSliderArgs.TickResolution.Get().AsDecimal() * TimeSliderArgs.DisplayRate.Get().AsInterval();
}

double SActActionTimeRange::ViewEndTime() const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = TimeSliderController.Pin()->GetTimeSliderArgs();
	const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	// View range is in seconds so we convert it to tick resolution
	const FFrameTime Time = TimeSliderArgs.ViewRange.Get().GetUpperBoundValue() * TickResolution;
	return Time.GetFrame().Value;
}

void SActActionTimeRange::OnViewEndTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const
{
	OnViewTimeChanged(NewValue, true);
}
#undef LOCTEXT_NAMESPACE
