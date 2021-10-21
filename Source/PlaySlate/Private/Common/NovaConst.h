#pragma once


namespace NovaConst
{
	const FName ActActionSequenceTabId(TEXT("ActAction_Sequence"));
	const FName ActActionViewportTabId(TEXT("ActAction_Viewport"));
	const FName ActActionDetailsViewTabId(TEXT("ActAction_DetailsView"));
	/** Notify State 最短持续时间，单位帧 */
	const int ActMinimumNotifyStateFrame = 1;
	/** Notify 高度偏移 */
	const float NotifyHeightOffset = 0.f;
	/** Notify 高度 */
	const float NotifyHeight = 24.0f;
}
