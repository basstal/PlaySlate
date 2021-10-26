// #pragma once
//
// #include "IContentBrowserSingleton.h"
// #include "Common/NovaDataBinding.h"
// #include "Common/NovaStruct.h"
//
// class FNovaActEditor;
// class FActEventTimelineSlider;
// class SActEventTimelineWidget;
// class ASkeletalMeshActor;
// class UActAnimation;
// class FActActionSequenceTreeViewNode;
// class SActActionViewportWidget;
// class FActEventTimelineImage;
//
// using namespace NovaStruct;
//
// class FActEventTimeline : public TSharedFromThis<FActEventTimeline>, FTickableEditorObject
// {
// public:
// 	FActEventTimeline();
// 	virtual ~FActEventTimeline() override;
//
// 	/**
// 	 * @param InParentDockTab 子Widget所附着的DockTab
// 	 */
// 	void Init(const TSharedRef<SDockTab>& InParentDockTab);
//
// 	//~Begin FTickableEditorObject interface
// 	virtual void Tick(float DeltaTime) override;
// 	virtual TStatId GetStatId() const override;
// 	//~End FTickableEditorObject interface
//
// 	
// protected:
// 	TSharedPtr<SActEventTimelineWidget> ActEventTimelineWidget;              // ** 保持Widget的引用以防止GC
// 	
//
// 	
// };
