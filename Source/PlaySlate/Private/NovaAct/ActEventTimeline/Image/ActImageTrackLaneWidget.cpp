// #include "ActImageTrackLaneWidget.h"
//
// // #include "PlaySlate.h"
// #include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
// // #include "NovaAct/ActEventTimeline/Image/ActImageTrackCarWidget.h"
//
// #include "ActImageTrackCarWidget.h"
// #include "Animation/EditorNotifyObject.h"
// #include "NovaAct/Assets/ActAnimation.h"
// #include "Widgets/SWeakWidget.h"
//
// SActImageTrackLaneWidget::Slot::Slot(const TSharedRef<SActImageTrackLaneWidget>& InSlotContent)
// {
// 	SlotContent = InSlotContent;
//
// 	HAlignment = HAlign_Fill;
// 	VAlignment = VAlign_Top;
//
// 	AttachWidget(
// 		SNew(SWeakWidget)
// 		.Clipping(EWidgetClipping::ClipToBounds)
// 		.PossiblyNullContent(InSlotContent)
// 	);
// }
//
// float SActImageTrackLaneWidget::Slot::GetVerticalOffset() const
// {
// 	if (SlotContent.IsValid())
// 	{
// 		return SlotContent->GetPhysicalPosition();
// 	}
// 	return 0.0f;
// }
//
// void SActImageTrackLaneWidget::Construct(const FArguments& InArgs, const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow)
// {
// 	Height = 24.0f;
//
// 	ActImageTreeViewTableRow = InActImageTreeViewTableRow;
//
// 	ChildSlot
// 	[
// 		SAssignNew(ActImageTrackCarWidgetBorder, SBorder)
// 		.Visibility(EVisibility::SelfHitTestInvisible)
// 		.AddMetaData<FTagMetaData>(TEXT("AnimNotify.Notify"))
// 		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
// 		.Padding(0.0f)
// 		.ColorAndOpacity(FLinearColor::White)
// 	];
// }
//
// int32 SActImageTrackLaneWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
// {
// 	// static const FName BorderName("AnimTimeline.Outliner.DefaultBorder");
// 	// static const FName SelectionColorName("SelectionColor");
//
// 	// TSharedPtr<FAnimTimelineTrack> Track = WeakTrack.Pin();
// 	// if(Track.IsValid())
// 	// {
// 	// 	if (Track->IsVisible())
// 	// 	{
// 	// float TotalNodeHeight = Track->GetHeight() + Track->GetPadding().Combined();
// 	//
// 	// // draw hovered
// 	// if (Track->IsHovered())
// 	// {
// 	// 	FSlateDrawElement::MakeBox(
// 	// 		OutDrawElements,
// 	// 		LayerId++,
// 	// 		AllottedGeometry.ToPaintGeometry(
// 	// 			FVector2D(0, 0),
// 	// 			FVector2D(AllottedGeometry.GetLocalSize().X, TotalNodeHeight)
// 	// 		),
// 	// 		FEditorStyle::GetBrush(BorderName),
// 	// 		ESlateDrawEffect::None,
// 	// 		FLinearColor(1.0f, 1.0f, 1.0f, 0.05f)
// 	// 	);
// 	// }
//
// 	// Draw track bottom border
// 	FSlateDrawElement::MakeLines(
// 		OutDrawElements,
// 		LayerId++,
// 		AllottedGeometry.ToPaintGeometry(),
// 		TArray<FVector2D>({
// 			FVector2D(0.0f, Height),
// 			FVector2D(AllottedGeometry.GetLocalSize().X, Height)
// 		}),
// 		ESlateDrawEffect::None,
// 		FLinearColor::White.CopyWithNewOpacity(0.2f)
// 	);
// 	// }
// 	//
// 	// }
//
// 	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 1, InWidgetStyle, bParentEnabled);
// }
//
// FVector2D SActImageTrackLaneWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
// {
// 	return FVector2D(100.0f, Height);
// }
//
// // void SActImageTrackLaneWidget::AddTrackCarWidget()
// // {
// // 	// TSharedRef<SActImageTrackCarWidget> ActImageTrackCarWidget = SNew(SActImageTrackCarWidget);
// // 	// // [
// // 	// // 	ActImageTreeViewTableRow.Pin()->GetActActionSectionWidget()
// // 	// // ];
// // 	// ActImageTrackCarWidgets.Add(ActImageTrackCarWidget);
// //
// // 	// ChildSlot.AttachWidget(ActImageTrackCarWidget);
// // 	// TSharedRef<SWeakWidget> AttachedWidget = SNew(SWeakWidget)
// // 	// .Clipping(EWidgetClipping::ClipToBounds)
// // 	// .PossiblyNullContent(ActImageTrackCarWidget);
// // 	// AttachWidget(AttachedWidget);
// // }
//
// // void SActImageTrackLaneWidget::MakeTrackLane()
// // {
// // 	// TrackLane = SNew(SActImageTrackCarWidget)
// // 	// [
// // 	// 	ActImageTreeViewTableRow.Pin()->GetActActionSectionWidget()
// // 	// ];
// // 	// TSharedRef<SWeakWidget> AttachedWidget = SNew(SWeakWidget)
// // 	// .Clipping(EWidgetClipping::ClipToBounds)
// // 	// .PossiblyNullContent(TrackLane);
// // 	// AttachWidget(AttachedWidget);
// // }
//
//
// FText SActImageTrackLaneWidget::GetNodeTooltip()
// {
// 	return FText();
// }
//
// float SActImageTrackLaneWidget::GetPlayLength()
// {
// 	// FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
// 	// ** TODO;
// 	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
// 	return 0;
// }
//
// FName SActImageTrackLaneWidget::GetName()
// {
// 	return NAME_None;
// }
//
// FLinearColor SActImageTrackLaneWidget::GetEditorColor()
// {
// 	return FLinearColor(1, 1, 0.5f);
// }
//
// //
// // void SActImageTrackLaneWidget::GetTime(float& OutTime, int32& OutFrame)
// // {
// // 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// // 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// // 	OutTime = 0;
// // 	OutFrame = 0;
// // 	if (ActImageTreeViewTableRow.Pin()->GetType() == EActImageTrackType::State)
// // 	{
// // 		OutTime = FMath::Max(OutTime, (float)(TrackAreaArgs.Begin.Get() * FrameRate.AsInterval()));
// // 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.Begin.Get());
// // 	}
// // }
// //
// // void SActImageTrackLaneWidget::GetDuration(float& OutTime, int32& OutFrame)
// // {
// // 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// // 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// // 	OutTime = NovaConst::ActMinimumNotifyStateFrame * FrameRate.AsInterval();
// // 	OutFrame = NovaConst::ActMinimumNotifyStateFrame;
// // 	if (ActImageTreeViewTableRow.Pin()->GetType() == EActImageTrackType::State)
// // 	{
// // 		OutTime = FMath::Max(OutTime, (float)((TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get()) * FrameRate.AsInterval()));
// // 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get());
// // 	}
// // }
//
// bool SActImageTrackLaneWidget::IsBranchingPoint()
// {
// 	return true;
// }
//
// bool SActImageTrackLaneWidget::HasNotifyNode()
// {
// 	// EActImageTrackType TableRowType = ActImageTreeViewTableRow.Pin()->GetType();
// 	// return TableRowType != EActImageTrackType::None && TableRowType != EActImageTrackType::Folder;
// 	return true;
// }
//
// // void SActImageTrackLaneWidget::SetVisibility(EVisibility InVisibility)
// // {
// // 	TrackLane->SetVisibility(InVisibility);
// // }
//
//
// float SActImageTrackLaneWidget::GetPhysicalPosition() const
// {
// 	if (ActImageTreeViewTableRow.IsValid())
// 	{
// 		return ActImageTreeViewTableRow.Pin()->ComputeTrackPosition();
// 	}
// 	return 0.0f;
// }
//
//
// // void SActImageTrackLaneWidget::RefreshNotifyTracks()
// // {
// // 	// FScopedSavedNotifySelection ScopedSelection(*this);
// //
// // 	// TSharedPtr<SVerticalBox> NotifySlots;
// // 	// ActImageTrackCarWidgetBorder->SetContent(SAssignNew(NotifySlots, SVerticalBox));
// // 	//
// // 	// // Clear node tool tips to stop slate referencing them and possibly
// // 	// // causing a crash if the notify has gone away
// // 	// // for (TSharedPtr<SActImageTrackCarWidget> Track : ActImageTrackCarWidgets)
// // 	// // {
// // 	// // 	Track->ClearNodeTooltips();
// // 	// // }
// // 	//
// // 	// ActImageTrackCarWidgets.Empty();
// // 	// // NotifyEditorTracks.Empty();
// // 	//
// // 	// auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
// // 	// UAnimSequence* AnimSequence = *(DB->GetData());
// // 	// int32 NotifyCount = AnimSequence->AnimNotifyTracks.Num();
// // 	// for (int32 Index = 0; Index < NotifyCount; Index++)
// // 	// {
// // 	// 	// FAnimNotifyTrack& Track = AnimSequence->AnimNotifyTracks[TrackIndex];
// // 	// 	TSharedPtr<SActImageTrackCarWidget> TrackCar;
// // 	//
// // 	// 	NotifySlots->AddSlot()
// // 	// 	           .AutoHeight()
// // 	// 	           .VAlign(VAlign_Center)
// // 	// 	[
// // 	// 		SAssignNew(TrackCar, SActImageTrackCarWidget)
// // 	// 		.TrackIndex(Index)
// // 	// 		// .Sequence(Sequence)
// // 	// 		// .AnimNotifyPanel(SharedThis(this))
// // 	// 		// .WidgetWidth(WidgetWidth)
// // 	// 		// .ViewInputMin(ViewInputMin)
// // 	// 		// .ViewInputMax(ViewInputMax)
// // 	// 		// .OnGetScrubValue(OnGetScrubValue)
// // 	// 		// .OnGetDraggedNodePos(this, &SAnimNotifyPanel::CalculateDraggedNodePos)
// // 	// 		// .OnUpdatePanel(this, &SAnimNotifyPanel::Update)
// // 	// 		// .OnGetNotifyBlueprintData(this, &SAnimNotifyPanel::OnGetNotifyBlueprintData, &NotifyClassNames)
// // 	// 		// .OnGetNotifyStateBlueprintData(this, &SAnimNotifyPanel::OnGetNotifyBlueprintData, &NotifyStateClassNames)
// // 	// 		// .OnGetNotifyNativeClasses(this, &SAnimNotifyPanel::OnGetNativeNotifyData, UAnimNotify::StaticClass(), &NotifyClassNames)
// // 	// 		// .OnGetNotifyStateNativeClasses(this, &SAnimNotifyPanel::OnGetNativeNotifyData, UAnimNotifyState::StaticClass(), &NotifyStateClassNames)
// // 	// 		// .OnSelectionChanged(this, &SAnimNotifyPanel::OnTrackSelectionChanged)
// // 	// 		// .OnNodeDragStarted(this, &SAnimNotifyPanel::OnNotifyNodeDragStarted)
// // 	// 		// .OnNotifyStateHandleBeingDragged(OnNotifyStateHandleBeingDragged)
// // 	// 		// .OnSnapPosition(OnSnapPosition)
// // 	// 		// .OnRequestRefreshOffsets(OnRequestRefreshOffsets)
// // 	// 		// .OnDeleteNotify(this, &SAnimNotifyPanel::DeleteSelectedNodeObjects)
// // 	// 		// .OnDeselectAllNotifies(this, &SAnimNotifyPanel::DeselectAllNotifies)
// // 	// 		// .OnCopyNodes(this, &SAnimNotifyPanel::CopySelectedNodesToClipboard)
// // 	// 		// .OnPasteNodes(this, &SAnimNotifyPanel::OnPasteNodes)
// // 	// 		// .OnSetInputViewRange(this, &SAnimNotifyPanel::InputViewRangeChanged)
// // 	// 		// .OnGetTimingNodeVisibility(OnGetTimingNodeVisibility)
// // 	// 		// .OnInvokeTab(OnInvokeTab)
// // 	// 	];
// // 	//
// // 	// 	ActImageTrackCarWidgets.Add(TrackCar.ToSharedRef());
// // 	// }
// // 	//
// // 	// // Signal selection change to refresh details panel
// // 	// OnTrackSelectionChanged();
// // }
//
//
// void SActImageTrackLaneWidget::OnTrackSelectionChanged()
// {
// 	if (!bIsSelecting)
// 	{
// 		TGuardValue<bool> GuardValue(bIsSelecting, true);
//
// 		// Need to collect selection info from all tracks
// 		TArray<UObject*> NotifyObjects;
//
// 		for (auto& TrackCarWidget : ActImageTrackCarWidgets)
// 		{
// 			// TSharedPtr<SAnimNotifyTrack> Track = NotifyAnimTracks[TrackIdx];
// 			const TArray<int32>& TrackIndices = TrackCarWidget->GetSelectedNotifyIndices();
// 			for (int32 Idx : TrackIndices)
// 			{
// 				TSharedRef<FActImageTrackCarNotifyNode> NotifyNodeInterface = TrackCarWidget->GetActImageTrackCarNotifyNode();
// 				if (NotifyNodeInterface->NotifyEvent)
// 				{
// 					auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
// 					UAnimSequence* AnimSequence = *(DB->GetData());
// 					FString ObjName = MakeUniqueObjectName(GetTransientPackage(), UEditorNotifyObject::StaticClass()).ToString();
// 					UEditorNotifyObject* NewNotifyObject = NewObject<UEditorNotifyObject>(GetTransientPackage(),
// 					                                                                      FName(*ObjName),
// 					                                                                      RF_Public | RF_Standalone | RF_Transient);
// 					// ** nullptr TODO:
// 					NewNotifyObject->InitFromAnim(AnimSequence, nullptr);
// 					NewNotifyObject->InitialiseNotify(*AnimSequence->AnimNotifyTracks[Idx].Notifies[Idx]);
// 					NotifyObjects.AddUnique(NewNotifyObject);
// 				}
// 			}
// 		}
//
// 		// OnSelectionChanged.ExecuteIfBound(NotifyObjects);
// 	}
// }
