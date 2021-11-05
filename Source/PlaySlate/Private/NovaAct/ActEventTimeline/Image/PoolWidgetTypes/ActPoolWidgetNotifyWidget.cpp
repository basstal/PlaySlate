#include "ActPoolWidgetNotifyWidget.h"

#include "StatusBarSubsystem.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActNotifyPoolEditorLaneWidget.h"
#include "NovaAct/ActEventTimeline/Image/Subs/NovaActUICommandInfo.h"
#include "Animation/EditorNotifyObject.h"
#include "Common/NovaConst.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTrackCarWidget.h"
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackBase.h"
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackNotify.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActNotifyPoolNotifyNodeWidget.h"
#include "NovaAct/ActEventTimeline/Operation/ActTrackAreaSlotDragDrop.h"

#define LOCTEXT_NAMESPACE "NovaAct"

using namespace NovaConst;

SActPoolWidgetNotifyWidget::~SActPoolWidgetNotifyWidget()
{
	NovaDB::Delete("ActPoolNotify");
}

void SActPoolWidgetNotifyWidget::Construct(const FArguments& InArgs, const TSharedRef<FActImageTrackNotify>& InActImageTrackNotify)
{
	TSharedPtr<SActPoolWidgetNotifyWidget> Pool = SharedThis(this);
	NovaDB::CreateSP("ActPoolNotify", Pool);
	// SAnimTrackPanel::Construct(SAnimTrackPanel::FArguments()
	//                            .WidgetWidth(InArgs._WidgetWidth)
	//                            .ViewInputMin(InArgs._ViewInputMin)
	//                            .ViewInputMax(InArgs._ViewInputMax)
	//                            .InputMin(InArgs._InputMin)
	//                            .InputMax(InArgs._InputMax)
	//                            .OnSetInputViewRange(InArgs._OnSetInputViewRange));

	// WeakModel = InModel;
	// WeakCommandList = InModel->GetCommandList();
	// Sequence = InArgs._Sequence;
	// OnInvokeTab = InArgs._OnInvokeTab;
	// OnNotifiesChanged = InArgs._OnNotifiesChanged;
	// OnSnapPosition = InArgs._OnSnapPosition;
	// OnNotifyStateHandleBeingDragged = InArgs._OnNotifyStateHandleBeingDragged;
	// OnNotifyNodesBeingDragged = InArgs._OnNotifyNodesBeingDragged;
	ActImageTrackNotify = InActImageTrackNotify;

	bIsSelecting = false;
	bIsUpdating = false;

	// InModel->OnHandleObjectsSelected().AddSP(this, &SAnimNotifyPanel::HandleObjectsSelected);

	FNovaActNotifiesPanelCommands::Register();

	auto HostAppDB = GetDataBindingSP(FNovaActEditor, "NovaActEditor");
	TSharedRef<FUICommandList> CommandList = HostAppDB->GetData()->GetToolkitCommands();
	const FNovaActNotifiesPanelCommands& Commands = FNovaActNotifiesPanelCommands::Get();
	CommandList->MapAction(
		Commands.DeleteNotify,
		FExecuteAction::CreateSP(this, &SActPoolWidgetNotifyWidget::OnDeletePressed));

	// CommandList->MapAction(
	// 	Commands.CopyNotifies,
	// 	FExecuteAction::CreateSP(this, &SActPoolWidgetNotifyWidget::CopySelectedNodesToClipboard));

	// CommandList->MapAction(
	// 	Commands.PasteNotifies,
	// 	FExecuteAction::CreateSP(this, &SActPoolWidgetNotifyWidget::OnPasteNodes, (SAnimNotifyTrack*)nullptr, -1.0f, ENotifyPasteMode::MousePosition, ENotifyPasteMultipleMode::Absolute));

	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
	AnimSequenceBase->RegisterOnNotifyChanged(
		UAnimSequenceBase::FOnNotifyChanged::CreateSP(this, &SActPoolWidgetNotifyWidget::RefreshNotifyTracks));
	//
	// InModel->GetEditableSkeleton()->RegisterOnNotifiesChanged(FSimpleDelegate::CreateSP(this, &SAnimNotifyPanel::RefreshNotifyTracks));
	// InModel->OnTracksChanged().Add(FSimpleDelegate::CreateSP(this, &SAnimNotifyPanel::RefreshNotifyTracks));

	if (GEditor)
	{
		GEditor->RegisterForUndo(this);
	}

	// CurrentPosition = InArgs._CurrentPosition;
	// OnSelectionChanged = InArgs._OnSelectionChanged;
	// WidgetWidth = InArgs._WidgetWidth;
	// OnGetScrubValue = InArgs._OnGetScrubValue;
	// OnRequestRefreshOffsets = InArgs._OnRequestRefreshOffsets;
	// OnGetTimingNodeVisibility = InArgs._OnGetTimingNodeVisibility;

	this->ChildSlot
	[
		SAssignNew(PanelArea, SBorder)
		.Visibility(EVisibility::SelfHitTestInvisible)
		.AddMetaData<FTagMetaData>(TEXT("AnimNotify.Notify"))
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.Padding(0.0f)
		.ColorAndOpacity(FLinearColor::White)
	];
	//
	// OnPropertyChangedHandle = FCoreUObjectDelegates::FOnObjectPropertyChanged::FDelegate::CreateSP(this, &SAnimNotifyPanel::OnPropertyChanged);
	// OnPropertyChangedHandleDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.Add(OnPropertyChangedHandle);
	//
	// // Base notify classes used to search asset data for children.
	// NotifyClassNames.Add(TEXT("Class'/Script/Engine.AnimNotify'"));
	// NotifyStateClassNames.Add(TEXT("Class'/Script/Engine.AnimNotifyState'"));
	//
	// PopulateNotifyBlueprintClasses(NotifyClassNames);
	// PopulateNotifyBlueprintClasses(NotifyStateClassNames);
	//
	// Update();

	FDelegateHandle _;
	DataBindingSPBindRaw(IActImageTrackBase, "ActImageTrack/Refresh", this, &SActPoolWidgetNotifyWidget::OnLaneContentRefresh, _)
}

FVector2D SActPoolWidgetNotifyWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100.0f, ActImageTrackNotify->ActImageTrackArgs->Height);
}

void SActPoolWidgetNotifyWidget::OnDeletePressed()
{
	// If there's no focus on the panel it's likely the user is not editing notifies
	// so don't delete anything when the key is pressed.
	if (HasKeyboardFocus() || HasFocusedDescendants())
	{
		DeleteSelectedNodeObjects();
	}
}

void SActPoolWidgetNotifyWidget::DeleteSelectedNodeObjects()
{
	// TArray<INodeObjectInterface*> SelectedNodes;
	// for (TSharedPtr<SAnimNotifyTrack> Track : NotifyLanes)
	// {
	// 	Track->AppendSelectionToArray(SelectedNodes);
	// }
	//
	// const bool bContainsSyncMarkers = SelectedNodes.ContainsByPredicate([](const INodeObjectInterface* Interface) { return Interface->GetType() == ENodeObjectTypes::NOTIFY; });
	//
	// if (SelectedNodes.Num() > 0)
	// {
	// 	FScopedTransaction Transaction(LOCTEXT("DeleteMarkers", "Delete Animation Markers"));
	// 	Sequence->Modify(true);
	//
	// 	// As we address node object's source data by pointer, we need to mark for delete then
	// 	// delete invalid entries to avoid concurrent modification of containers
	// 	for (INodeObjectInterface* NodeObject : SelectedNodes)
	// 	{
	// 		NodeObject->MarkForDelete(Sequence);
	// 	}
	//
	// 	FNotifyNodeInterface::RemoveInvalidNotifies(Sequence);
	// 	FSyncMarkerNodeInterface::RemoveInvalidSyncMarkers(Sequence);
	//
	// 	if (bContainsSyncMarkers)
	// 	{
	// 		UBlendSpaceBase::UpdateBlendSpacesUsingAnimSequence(Sequence);
	// 	}
	// }
	//
	// // clear selection and update the panel
	// TArray<UObject*> Objects;
	// OnSelectionChanged.ExecuteIfBound(Objects);
	//
	// Update();
}

void SActPoolWidgetNotifyWidget::RefreshNotifyTracks()
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
	if (AnimSequenceBase)
	{
		// FScopedSavedNotifySelection ScopedSelection(*this);

		TSharedPtr<SVerticalBox> NotifySlots;
		PanelArea->SetContent(
			SAssignNew(NotifySlots, SVerticalBox)
		);

		// Clear node tool tips to stop slate referencing them and possibly
		// causing a crash if the notify has gone away
		// for (TSharedPtr<SActNotifyPoolLaneWidget> Track : NotifyLanes)
		// {
		// 	// Track->ClearNodeTooltips();
		// }

		NotifyLanes.Empty();
		NotifyEditorTracks.Empty();

		for (int32 TrackIndex = 0; TrackIndex < AnimSequenceBase->AnimNotifyTracks.Num(); TrackIndex++)
		{
			// FAnimNotifyTrack& Track = AnimSequenceBase->AnimNotifyTracks[TrackIndex];
			TSharedRef<SActNotifyPoolEditorLaneWidget> EditorLaneWidget =
				SNew(SActNotifyPoolEditorLaneWidget)
				.TrackIndex(TrackIndex);
			NotifySlots->AddSlot()
			           .AutoHeight()
			           .VAlign(VAlign_Center)
			[
				EditorLaneWidget

				// .Sequence(AnimSequenceBase)
				// .AnimNotifyPanel(SharedThis(this))
				// .WidgetWidth(WidgetWidth)
				// .ViewInputMin(ViewInputMin)
				// .ViewInputMax(ViewInputMax)
				// .OnGetScrubValue(OnGetScrubValue)
				// .OnGetDraggedNodePos(this, &SAnimNotifyPanel::CalculateDraggedNodePos)
				// .OnUpdatePanel(this, &SAnimNotifyPanel::Update)
				// .OnGetNotifyBlueprintData(this, &SAnimNotifyPanel::OnGetNotifyBlueprintData, &NotifyClassNames)
				// .OnGetNotifyStateBlueprintData(this, &SAnimNotifyPanel::OnGetNotifyBlueprintData, &NotifyStateClassNames)
				// .OnGetNotifyNativeClasses(this, &SAnimNotifyPanel::OnGetNativeNotifyData, UAnimNotify::StaticClass(), &NotifyClassNames)
				// .OnGetNotifyStateNativeClasses(this, &SAnimNotifyPanel::OnGetNativeNotifyData, UAnimNotifyState::StaticClass(), &NotifyStateClassNames)
				// .OnSelectionChanged(this, &SAnimNotifyPanel::OnTrackSelectionChanged)
				// .OnNodeDragStarted(this, &SAnimNotifyPanel::OnNotifyNodeDragStarted)
				// .OnNotifyStateHandleBeingDragged(OnNotifyStateHandleBeingDragged)
				// .OnSnapPosition(OnSnapPosition)
				// .OnRequestRefreshOffsets(OnRequestRefreshOffsets)
				// .OnDeleteNotify(this, &SAnimNotifyPanel::DeleteSelectedNodeObjects)
				// .OnDeselectAllNotifies(this, &SAnimNotifyPanel::DeselectAllNotifies)
				// .OnCopyNodes(this, &SAnimNotifyPanel::CopySelectedNodesToClipboard)
				// .OnPasteNodes(this, &SAnimNotifyPanel::OnPasteNodes)
				// .OnSetInputViewRange(this, &SAnimNotifyPanel::InputViewRangeChanged)
				// .OnGetTimingNodeVisibility(OnGetTimingNodeVisibility)
				// .OnInvokeTab(OnInvokeTab)
			];

			NotifyLanes.Add(EditorLaneWidget->GetNotifyTrack());
			NotifyEditorTracks.Add(EditorLaneWidget);
		}
	}

	// Signal selection change to refresh details panel
	OnTrackSelectionChanged();
}


void SActPoolWidgetNotifyWidget::OnTrackSelectionChanged()
{
	// if (!bIsSelecting)
	// {
	// 	TGuardValue<bool> GuardValue(bIsSelecting, true);
	//
	// 	// Need to collect selection info from all tracks
	// 	TArray<UObject*> NotifyObjects;
	//
	// 	for (auto& TrackCarWidget : ActImageTrackCarWidgets)
	// 	{
	// 		// TSharedPtr<SAnimNotifyTrack> Track = NotifyAnimTracks[TrackIdx];
	// 		const TArray<int32>& TrackIndices = TrackCarWidget->GetSelectedNotifyIndices();
	// 		for (int32 Idx : TrackIndices)
	// 		{
	// 			TSharedRef<FActImageTrackCarNotifyNode> NotifyNodeInterface = TrackCarWidget->GetActImageTrackCarNotifyNode();
	// 			if (NotifyNodeInterface->NotifyEvent)
	// 			{
	// 				auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	// 				UAnimSequence* AnimSequence = *(DB->GetData());
	// 				FString ObjName = MakeUniqueObjectName(GetTransientPackage(), UEditorNotifyObject::StaticClass()).ToString();
	// 				UEditorNotifyObject* NewNotifyObject = NewObject<UEditorNotifyObject>(GetTransientPackage(),
	// 				                                                                      FName(*ObjName),
	// 				                                                                      RF_Public | RF_Standalone | RF_Transient);
	// 				// ** nullptr TODO:
	// 				NewNotifyObject->InitFromAnim(AnimSequence, nullptr);
	// 				NewNotifyObject->InitialiseNotify(*AnimSequence->AnimNotifyTracks[Idx].Notifies[Idx]);
	// 				NotifyObjects.AddUnique(NewNotifyObject);
	// 			}
	// 		}
	// 	}
	//
	// 	// OnSelectionChanged.ExecuteIfBound(NotifyObjects);
	// }
}

void SActPoolWidgetNotifyWidget::OnLaneContentRefresh(TSharedPtr<IActImageTrackBase> InActImageTrack)
{
	if (InActImageTrack != ActImageTrackNotify)
	{
		return;
	}
	if (!bIsUpdating)
	{
		auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
		if (!DB)
		{
			return;
		}
		TGuardValue<bool> ScopeGuard(bIsUpdating, true);

		UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
		if (AnimSequenceBase)
		{
			AnimSequenceBase->RefreshCacheData();
		}

		RefreshNotifyTracks();

		// OnNotifiesChanged.ExecuteIfBound();
	}
}


FReply SActPoolWidgetNotifyWidget::OnNotifyNodeDragStarted(const TSharedRef<SActNotifyPoolLaneWidget>& InLane,
                                                           const TSharedRef<SActNotifyPoolNotifyNodeWidget>& InNotifyNode,
                                                           const FPointerEvent& InMouseEvent,
                                                           const bool bDragOnMarker)
{
	SelectNotifyNode(InNotifyNode, InMouseEvent.IsShiftDown(), false);

	// If we're dragging one of the direction markers we don't need to call any further as we don't want the drag drop op
	if (!bDragOnMarker)
	{
		TSharedRef<SOverlay> NodeDragDecoratorOverlay = SNew(SOverlay);
		TSharedRef<SBorder> NodeDragDecorator = SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				NodeDragDecoratorOverlay
			];

		TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>> NotifyNodes;
		for (TSharedPtr<SActNotifyPoolLaneWidget> Lane : NotifyLanes)
		{
			Lane->DisconnectSelectedNodesForDrag(NotifyNodes);
		}
		FBox2D OverlayBounds;
		for (const TSharedPtr<SActNotifyPoolNotifyNodeWidget>& NotifyNode : NotifyNodes)
		{
			OverlayBounds += FBox2D(NotifyNode->ScreenPosition, NotifyNode->ScreenPosition + FVector2D(NotifyNode->DurationSizeX, 0.0f));
		}
		FVector2D OverlayOrigin = OverlayBounds.Min;
		FVector2D OverlayExtents = OverlayBounds.GetSize();
		for (const TSharedPtr<SActNotifyPoolNotifyNodeWidget>& NotifyNode : NotifyNodes)
		{
			FVector2D OffsetFromMin(NotifyNode->ScreenPosition - OverlayOrigin);
			NodeDragDecoratorOverlay->AddSlot()
			                        .Padding(FMargin(OffsetFromMin.X, OffsetFromMin.Y, 0.0f, 0.0f))
			[
				NotifyNode->AsShared()
			];
		}

		// ** TODO: replace with StaticFunction :: OnViewRangePan
		// FPanTrackRequest PanRequestDelegate = FPanTrackRequest::CreateSP(this, &SAnimNotifyPanel::PanInputViewRange);
		// ** TODO: replace with "ActImageTrack/Refresh" DataBinding Trigger
		// FOnUpdatePanel UpdateDelegate = FOnUpdatePanel::CreateSP(this, &SAnimNotifyPanel::Update);
		return FReply::Handled().BeginDragDrop(FActTrackAreaSlotDragDrop::New(SharedThis(this),
		                                                                      NotifyNodes,
		                                                                      NodeDragDecorator,
		                                                                      InMouseEvent.GetScreenSpacePosition(),
		                                                                      OverlayOrigin,
		                                                                      OverlayExtents,
		                                                                      CurrentDragXPosition));
	}
	else
	{
		// Capture the mouse in the node
		return FReply::Handled().CaptureMouse(InNotifyNode).UseHighPrecisionMouseMovement(InNotifyNode);
	}
}


void SActPoolWidgetNotifyWidget::SelectNotifyNode(const TSharedRef<SActNotifyPoolNotifyNodeWidget>& NotifyNode,
                                                  bool Append,
                                                  bool bUpdateSelection)
{
	// Deselect all other notifies if necessary.
	if (!Append)
	{
		DeselectAllNotifies();
	}
	if (!SelectedNotifyNodes.Contains(NotifyNode))
	{
		SelectedNotifyNodes.Add(NotifyNode);
	}
	// NotifyNode->bSelected = true;
	// if (bUpdateSelection)
	// {
	// 	SendSelectionChanged();
	// }
}


void SActPoolWidgetNotifyWidget::DeselectAllNotifies()
{
	if (!bIsSelecting)
	{
		TGuardValue<bool> GuardValue(bIsSelecting, true);

		SelectedNotifyNodes.Empty();
		// for (TSharedPtr<SActNotifyPoolLaneWidget> Lane : NotifyLanes)
		// {
		// 	Lane->DeselectAllNotifyNodes(false);
		// }

		// TArray<UObject*> NotifyObjects;
		// OnSelectionChanged.ExecuteIfBound(NotifyObjects);
	}
}

void SActPoolWidgetNotifyWidget::OnNotifyNodesBeingDraggedStatusBarMessage()
{
	if (!StatusBarMessageHandle.IsValid())
	{
		if (UStatusBarSubsystem* StatusBarSubsystem = GEditor->GetEditorSubsystem<UStatusBarSubsystem>())
		{
			StatusBarMessageHandle = StatusBarSubsystem->PushStatusBarMessage(AnimationEditorStatusBarName,
			                                                                  LOCTEXT("AutoscrubNotify",
			                                                                          "Hold SHIFT while dragging a notify to auto scrub the timeline."));
		}
	}
}

#undef LOCTEXT_NAMESPACE
