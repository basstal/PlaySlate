#pragma once

#define LOCTEXT_NAMESPACE "NovaAct"

namespace NovaConst
{
	const FText NovaActEditor_ActEventTimeline = LOCTEXT("ActEventTimeline", "NovaActEditor ActEventTimeline tab name.");

	const FName NovaActAppName(TEXT("NovaAct"));
	const FName NovaActEditorMode(TEXT("NovaActEditorMode"));

	const FName ActEventTimelineTabId(TEXT("ActEventTimeline"));

	const TArray<FName> ActViewportTabIds = TArray<FName>({
		TEXT("ActViewport1"),
		TEXT("ActViewport2"),
		TEXT("ActViewport3"),
		TEXT("ActViewport4")
	});

	const FName ActViewportTab2Id = FName();
	const FName ActViewportTab3Id = FName();
	const FName ActViewportTab4Id = FName();
	
	const FName AnimationEditorStatusBarName = FName(TEXT("AssetEditor.AnimationEditor.MainMenu"));


	const FName ActAssetDetailsTabId(TEXT("ActAssetDetails"));
	// Notify State 最短持续时间， 单位秒
	const float MinimumStateDuration = (1.0f / 30.0f);
	/** Notify State 最短持续时间，单位帧 */
	const int32 ActMinimumNotifyStateFrame = 1;
	/** Notify 高度偏移 */
	const float NotifyHeightOffset = 0.f;
	/** Notify 高度 */
	const float NotifyHeight = 24.0f;
	/** Outliner 右侧 间隔距离 */
	const float OutlinerRightPadding = 8.0f;

	const FMargin ResizeBarPadding(4.0f, 0, 0, 0);

	/** FActViewportPreviewScene tick max threshold */
	const float VisibilityTimeThreshold = 0.25f;

	/** 给 NotifyNodeWidget 使用的常量 用于绘制UI */
	const FVector2D ScrubHandleSize = FVector2D(12.0f, 12.0f);
	const FVector2D TextBorderSize = FVector2D(1.f, 1.f);
}

#undef LOCTEXT_NAMESPACE
