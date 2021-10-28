#pragma once

class SActViewportTabWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActViewportTabWidget) {}
		// 	: _BlueprintEditor()
		// 	, _ContextName(NAME_None)
		// 	, _ShowShowMenu(true)
		// 	, _ShowLODMenu(true)
		// 	, _ShowPlaySpeedMenu(true)
		// 	, _ShowTimeline(true)
		// 	, _ShowStats(true)
		// 	, _AlwaysShowTransformToolbar(false)
		// 	, _ShowFloorOptions(true)
		// 	, _ShowTurnTable(true)
		// 	, _ShowPhysicsMenu(false)
		// {}
		//
		// SLATE_ARGUMENT(TWeakPtr<FBlueprintEditor>, BlueprintEditor)
		//
		// SLATE_ARGUMENT(FOnInvokeTab, OnInvokeTab)
		//
		// SLATE_ARGUMENT(TArray<TSharedPtr<FExtender>>, Extenders)
		//
		// SLATE_ARGUMENT(FOnGetViewportText, OnGetViewportText)
		//
		// SLATE_ARGUMENT(FName, ContextName)
		//
		// SLATE_ARGUMENT(bool, ShowShowMenu)
		//
		// SLATE_ARGUMENT(bool, ShowLODMenu)
		//
		// SLATE_ARGUMENT(bool, ShowPlaySpeedMenu)
		//
		// SLATE_ARGUMENT(bool, ShowTimeline)
		//
		// SLATE_ARGUMENT(bool, ShowStats)
		//
		// SLATE_ARGUMENT(bool, AlwaysShowTransformToolbar)
		//
		// SLATE_ARGUMENT(bool, ShowFloorOptions)
		//
		// SLATE_ARGUMENT(bool, ShowTurnTable)
		//
		// SLATE_ARGUMENT(bool, ShowPhysicsMenu)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs);
};
