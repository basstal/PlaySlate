﻿#pragma once

#include "FrameNumberNumericInterface.h"
#include "NovaDataBinding.h"
#include "Common/NovaStruct.h"
#include "Fonts/FontMeasure.h"

using namespace NovaStruct;

class NovaStaticFunction
{
public:
	static float GetBoundsZOffset(const FBoxSphereBounds& Bounds)
	{
		// Return half the height of the bounds plus one to avoid ZFighting with the floor plane
		return Bounds.BoxExtent.Z + 1;
	}

	static FFrameNumber DiscreteExclusiveUpper(const TRangeBound<FFrameNumber>& InUpperBound)
	{
		check(!InUpperBound.IsOpen());

		// Add one for inclusive upper bounds since they finish on the next subsequent frame
		static const int32 Offsets[] = {0, 1};
		const int32 OffsetIndex = (int32)InUpperBound.IsInclusive();

		return InUpperBound.GetValue() + Offsets[OffsetIndex];
	}

	static FFrameNumber DiscreteInclusiveLower(const TRangeBound<FFrameNumber>& InLowerBound)
	{
		check(!InLowerBound.IsOpen());

		// Add one for exclusive lower bounds since they start on the next subsequent frame
		static const int32 Offsets[] = {0, 1};
		const int32 OffsetIndex = (int32)InLowerBound.IsExclusive();

		return InLowerBound.GetValue() + Offsets[OffsetIndex];
	}

	/**
	 * 根据鼠标位置计算当前 时间轴刷 对应的动画时间
	 *
	 * @param Geometry 时间轴刷的 几何体
	 * @param ScreenSpacePosition 鼠标对应的 屏幕坐标 位置
	 * @param RangeToScreen 屏幕距离到时间的转换结构
	 * @param TickResolution 时间到帧时间的转换结构
	 */
	static FFrameTime ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, FActSliderScrubRangeToScreen RangeToScreen, const FFrameRate& TickResolution)
	{
		const FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
		const double MouseValue = RangeToScreen.LocalXToInput(CursorPos.X);
		return MouseValue * TickResolution;
	}

	/**
	 * @return Get the pixel metrics of the Scrubber
	 * @param DisplayRate		当前帧率
	 * @param ScrubTime			The qualified time of the scrubber
	 * @param RangeToScreen		Range to screen helper
	 */
	static FActSliderScrubberMetrics MakePixelScrubberMetrics(const FFrameRate& DisplayRate,
	                                                          const FQualifiedFrameTime& ScrubTime,
	                                                          const FActSliderScrubRangeToScreen& RangeToScreen)
	{
		FActSliderScrubberMetrics Metrics;

		const FFrameNumber Frame = ScrubTime.ConvertTo(DisplayRate).FloorToFrame();

		float FrameStartPixel = RangeToScreen.InputToLocalX(Frame / DisplayRate);
		float FrameEndPixel = RangeToScreen.InputToLocalX((Frame + 1) / DisplayRate) - 1;

		float RoundedStartPixel = FMath::RoundToInt(FrameStartPixel);
		FrameEndPixel -= (FrameStartPixel - RoundedStartPixel);

		FrameStartPixel = RoundedStartPixel;
		FrameEndPixel = FMath::Max(FrameEndPixel, FrameStartPixel + 1);

		// Store off the pixel width of the frame
		Metrics.FrameExtentsPx = TRange<float>(FrameStartPixel, FrameEndPixel);

		// Set the style of the scrub handle
		Metrics.Style = EActSliderScrubberStyle::Vanilla;

		// Always draw the extents on the section area for frame block styles
		Metrics.bDrawExtents = Metrics.Style == EActSliderScrubberStyle::FrameBlock;

		static float MinScrubSize = 14.f;
		// If it's vanilla style or too small to show the frame width, set that up
		if (Metrics.Style == EActSliderScrubberStyle::Vanilla || FrameEndPixel - FrameStartPixel < MinScrubSize)
		{
			Metrics.Style = EActSliderScrubberStyle::Vanilla;

			float ScrubPixel = RangeToScreen.InputToLocalX(ScrubTime.AsSeconds());
			Metrics.HandleRangePx = TRange<float>(ScrubPixel - MinScrubSize * .5f, ScrubPixel + MinScrubSize * .5f);
		}
		else
		{
			Metrics.HandleRangePx = Metrics.FrameExtentsPx;
		}

		return Metrics;
	}

	static bool GetGridMetrics(TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface,
	                           const float PhysicalWidth,
	                           const double InViewStart,
	                           const double InViewEnd,
	                           double& OutMajorInterval,
	                           int32& OutMinorDivisions)
	{
		FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
		TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

		// ** Use the end of the view as the longest number
		FFrameRate FocusedDisplayRate = FFrameRate();
		FString TickString = NumericTypeInterface->ToString((InViewEnd * FocusedDisplayRate).FrameNumber.Value);
		FVector2D MaxTextSize = FontMeasureService->Measure(TickString, SmallLayoutFont);

		float MinTickPx = MaxTextSize.X + 5.f;
		float DesiredMajorTickPx = MaxTextSize.X * 2.0f;

		if (PhysicalWidth > 0)
		{
			return FocusedDisplayRate.ComputeGridSpacing(
				PhysicalWidth / (InViewEnd - InViewStart),
				OutMajorInterval,
				OutMinorDivisions,
				MinTickPx,
				DesiredMajorTickPx);
		}

		return false;
	}
	
	static TSharedRef<SWidget> MakeTrackButton(FText HoverText, FOnGetContent MenuContent, const TAttribute<bool>& HoverState)
	{
		FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

		TSharedRef<STextBlock> ComboButtonText = SNew(STextBlock)
			.Text(HoverText)
			.Font(SmallLayoutFont)
			.ColorAndOpacity(FSlateColor::UseForeground());

		TSharedRef<SComboButton> ComboButton =

			SNew(SComboButton)
			.HasDownArrow(false)
			.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
			.ForegroundColor(FSlateColor::UseForeground())
			.OnGetMenuContent(MenuContent)
			.ContentPadding(FMargin(5, 2))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.ButtonContent()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				  .AutoWidth()
				  .VAlign(VAlign_Center)
				  .Padding(FMargin(0, 0, 2, 0))
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FEditorStyle::GetBrush("ComboButton.Arrow"))
				]

				+ SHorizontalBox::Slot()
				  .VAlign(VAlign_Center)
				  .AutoWidth()
				[
					ComboButtonText
				]
			];

		auto GetRolloverVisibility = [WeakComboButton = TWeakPtr<SComboButton>(ComboButton), HoverState]()
		{
			TSharedPtr<SComboButton> ComboButton = WeakComboButton.Pin();
			if (HoverState.Get() || ComboButton->IsOpen())
			{
				return EVisibility::SelfHitTestInvisible;
			}
			else
			{
				return EVisibility::Collapsed;
			}
		};

		TAttribute<EVisibility> Visibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda(GetRolloverVisibility));
		ComboButtonText->SetVisibility(Visibility);

		return ComboButton;
	}

	static TSharedPtr<FActEventTimelineArgs> MakeActEventTimelineArgs()
	{
		// ** EventTimeline 共享参数的初始化
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = MakeShareable(new FActEventTimelineArgs());
		auto TickResolutionAttrLambda = MakeAttributeLambda([]()
		{
			auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
			TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
			return ActEventTimelineArgs->TickResolution;
		});
		ActEventTimelineArgs->NumericTypeInterface = MakeShareable(new FFrameNumberInterface(MakeAttributeLambda([]()
																							 {
																								 return EFrameNumberDisplayFormats::Frames;
																							 }),
																							 0,
																							 TickResolutionAttrLambda,
																							 TickResolutionAttrLambda));
		return ActEventTimelineArgs;
	}
};
