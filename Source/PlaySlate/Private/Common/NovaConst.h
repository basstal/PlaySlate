#pragma once

namespace NovaConst
{
	const FName NovaActAppName(TEXT("NovaAct"));
	const FName NovaActEditorMode(TEXT("NovaActEditorMode"));

	const FName ActEventTimelineTabId(TEXT("ActEventTimeline"));
	
	const TArray<FName> ActViewportTabIds = TArray<FName>({
		TEXT("ActViewport1"),
		TEXT("ActViewport2"),
		TEXT("ActViewport3"),
		TEXT("ActViewport4")
	});
	
	const FName ActViewportTab2Id();
	const FName ActViewportTab3Id();
	const FName ActViewportTab4Id();
	
	const FName ActAssetDetailsTabId(TEXT("ActAssetDetails"));
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
}
