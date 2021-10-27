// #include "ActActionTimeSliderWidget.h"
//
// #include "Common/NovaDataBinding.h"
// #include "NovaAct/ActEventTimeline/Slider/ActEventTimelineSliderWidget.h"
//
// #include "Fonts/FontMeasure.h"
// #include "Common/NovaStaticFunction.h"
//
// void SActActionTimeSliderWidget::Construct(const FArguments& InArgs, const TSharedRef<SActEventTimelineSliderWidget>& InTimeSliderController)
// {
// 	TimeSliderController = InTimeSliderController;
// }
//
// int32 SActActionTimeSliderWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
// {
// 	
// }
//
//
// FReply SActActionTimeSliderWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
// {
// 	TimeSliderController.Pin()->OnMouseButtonDown(MyGeometry, MouseEvent);
// 	return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
// }
//
// FReply SActActionTimeSliderWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
// {
// 	return TimeSliderController.Pin()->OnMouseButtonUp(MyGeometry, MouseEvent);
// }
//
// FReply SActActionTimeSliderWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
// {
// 	return TimeSliderController.Pin()->OnMouseMove(MyGeometry, MouseEvent);
// }
//
// FReply SActActionTimeSliderWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
// {
// 	return TimeSliderController.Pin()->OnMouseWheel(MyGeometry, MouseEvent);
// }
