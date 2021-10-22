#pragma once


namespace NovaConst
{
	const FName NovaActAppName = FName(TEXT("NovaAct"));

	const FName ActEventTimelineTabId(TEXT("ActEventTimeline"));
	const FName ActViewportTabId(TEXT("ActViewport"));
	const FName ActAssetDetailsTabId(TEXT("ActAssetDetails"));
	/** Notify State 最短持续时间，单位帧 */
	const int ActMinimumNotifyStateFrame = 1;
	/** Notify 高度偏移 */
	const float NotifyHeightOffset = 0.f;
	/** Notify 高度 */
	const float NotifyHeight = 24.0f;
}
