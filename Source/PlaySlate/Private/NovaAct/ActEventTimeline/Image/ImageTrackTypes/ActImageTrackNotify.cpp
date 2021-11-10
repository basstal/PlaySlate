#include "ActImageTrackNotify.h"

#include "Animation/AnimMontage.h"
#include "Common/NovaConst.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/ActEventTimeline/Image/Subs/NovaActUICommandInfo.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "PlaySlate.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStaticFunction.h"
#include "NovaAct/ActEventTimeline/Image/PoolWidgetTypes/ActPoolWidgetNotifyWidget.h"

using namespace NovaConst;

#define LOCTEXT_NAMESPACE "NovaAct"

FActImageTrackNotify::FActImageTrackNotify()
	: PendingRenameTrackIndex(INDEX_NONE)
{
	ActImageTrackArgs = MakeShareable(new FActImageTrackArgs());
	{
		ActImageTrackArgs->TrackType = EActImageTrackType::Notify;
		ActImageTrackArgs->bSupportFiltering = true;
	}
}

FActImageTrackNotify::~FActImageTrackNotify()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActImageTrackNotify::~FActImageTrackNotify"));
	NovaDB::UnBind("ActImageTrack/Refresh", OnTreeViewContentRefreshHandle);
}

TSharedRef<SWidget> FActImageTrackNotify::GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow)
{
	TSharedRef<SHorizontalBox> GenerateContent = SNew(SHorizontalBox)
		.ToolTipText(FText::GetEmpty())
		+ SHorizontalBox::Slot()
		[
			SAssignNew(NotifyContainerBox, SVerticalBox)
		];

	auto DB = GetDataBindingSP(IActImageTrackBase, "ActImageTrack/Refresh");
	DB->SetData(SharedThis(this));
	DataBindingSPBindRaw(IActImageTrackBase,
	                     "ActImageTrack/Refresh",
	                     this,
	                     &FActImageTrackNotify::OnTreeViewContentRefresh,
	                     OnTreeViewContentRefreshHandle);
	return GenerateContent;
}

TSharedRef<SWidget> FActImageTrackNotify::GenerateContentWidgetForLaneWidget(const TSharedRef<SActImagePoolWidget>& InLaneWidget)
{
	return SNew(SActPoolWidgetNotifyWidget, SharedThis(this));
}

void FActImageTrackNotify::OnTreeViewContentRefresh(TSharedPtr<IActImageTrackBase> InActImageTrack)
{
	if (InActImageTrack.Get() != this)
	{
		return;
	}
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	NotifyContainerBox->ClearChildren();
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
	// ** Notify 轨道数量
	int32 NotifyTrackCount = AnimSequenceBase->AnimNotifyTracks.Num();
	for (int32 TrackIndex = 0; TrackIndex < NotifyTrackCount; ++TrackIndex)
	{
		TSharedPtr<SBox> SlotBox;
		NotifyContainerBox
			->AddSlot()
			.AutoHeight()
			[
				SAssignNew(SlotBox, SBox)
				.HeightOverride(NotifyHeight)
			];


		auto TextLambda = [TrackIndex]()
		{
			auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
			if (DB)
			{
				UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
				return AnimSequenceBase->AnimNotifyTracks.IsValidIndex(TrackIndex) ?
					       FText::FromName(AnimSequenceBase->AnimNotifyTracks[TrackIndex].TrackName) :
					       FText::GetEmpty();
			}
			return FText::GetEmpty();
		};
		TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock;
		TSharedPtr<SHorizontalBox> HorizontalBox;
		SlotBox->SetContent(
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
			.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
			[
				SAssignNew(HorizontalBox, SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .VAlign(VAlign_Center)
				  .HAlign(HAlign_Left)
				  .Padding(30.0f, 0.0f, 0.0f, 0.0f)
				[
					SAssignNew(InlineEditableTextBlock, SInlineEditableTextBlock)
					.Text_Lambda(TextLambda)
					.IsSelected_Lambda([] { return true; })
					.OnTextCommitted(FOnTextCommitted::CreateRaw(this, &FActImageTrackNotify::OnCommitName, TrackIndex))
				]

			]
		);

		UAnimMontage* AnimMontage = Cast<UAnimMontage>(AnimSequenceBase);
		// 非 Montage 资源且有 ParentAsset 的情况下 都可以添加 TrackLane
		if (!(AnimMontage && AnimMontage->HasParentAsset()))
		{
			HorizontalBox
				->AddSlot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.Padding(OutlinerRightPadding, 1.0f)
				[
					NovaStaticFunction::MakeTrackButton(LOCTEXT("AddTrackButtonText", "Track"),
					                                    FOnGetContent::CreateRaw(this,
					                                                             &FActImageTrackNotify::BuildSubMenu,
					                                                             TrackIndex),
					                                    MakeAttributeSP(SlotBox.Get(), &SWidget::IsHovered))
				];
		}

		if (PendingRenameTrackIndex == TrackIndex)
		{
			TWeakPtr<SInlineEditableTextBlock> WeakInlineEditableTextBlock = InlineEditableTextBlock;
			InlineEditableTextBlock->RegisterActiveTimer(0.0f,
			                                             FWidgetActiveTimerDelegate::CreateRaw(this,
			                                                                                   &FActImageTrackNotify::PendingRenameTimer,
			                                                                                   WeakInlineEditableTextBlock));
		}
	}
	ActImageTrackArgs->Height = NotifyTrackCount * NotifyHeight;
}


void FActImageTrackNotify::OnCommitName(const FText& InText, ETextCommit::Type CommitInfo, int32 TrackIndex)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	UAnimSequenceBase* AnimSequence = *(DB->GetData());
	if (AnimSequence->AnimNotifyTracks.IsValidIndex(TrackIndex))
	{
		FScopedTransaction Transaction(FText::Format(LOCTEXT("RenameNotifyTrack", "Rename Notify Track to '{0}'"), InText));
		AnimSequence->Modify();

		FText TrimText = FText::TrimPrecedingAndTrailing(InText);
		AnimSequence->AnimNotifyTracks[TrackIndex].TrackName = FName(*TrimText.ToString());
	}
}

TSharedRef<SWidget> FActImageTrackNotify::BuildSubMenu(int32 InTrackIndex)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	auto HostAppDB = GetDataBindingSP(FNovaActEditor, "NovaActEditor");
	if (!DB || !HostAppDB)
	{
		return SNullWidget::NullWidget;
	}
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
	FMenuBuilder MenuBuilder(true, HostAppDB->GetData()->GetToolkitCommands());

	MenuBuilder.BeginSection("NotifyTrack", LOCTEXT("NotifyTrackMenuSection", "Notify Track"));
	{
		MenuBuilder.AddMenuEntry(
			FNovaActUICommandInfo::Get().InsertNotifyTrack->GetLabel(),
			FNovaActUICommandInfo::Get().InsertNotifyTrack->GetDescription(),
			FNovaActUICommandInfo::Get().InsertNotifyTrack->GetIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FActImageTrackNotify::InsertNewTrack, InTrackIndex))
		);

		if (AnimSequenceBase->AnimNotifyTracks.Num() > 1)
		{
			MenuBuilder.AddMenuEntry(
				FNovaActUICommandInfo::Get().RemoveNotifyTrack->GetLabel(),
				FNovaActUICommandInfo::Get().RemoveNotifyTrack->GetDescription(),
				FNovaActUICommandInfo::Get().RemoveNotifyTrack->GetIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FActImageTrackNotify::RemoveTrack, InTrackIndex))
			);
		}
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}


void FActImageTrackNotify::InsertNewTrack(int32 InTrackIndexToInsert)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	UAnimSequenceBase* AnimSequence = *(DB->GetData());

	FScopedTransaction Transaction(LOCTEXT("InsertNotifyTrack", "Insert Notify Track"));
	AnimSequence->Modify();

	// before insert, make sure everything behind is fixed
	for (int32 TrackIndex = InTrackIndexToInsert; TrackIndex < AnimSequence->AnimNotifyTracks.Num(); ++TrackIndex)
	{
		FAnimNotifyTrack& Track = AnimSequence->AnimNotifyTracks[TrackIndex];

		const int32 NewTrackIndex = TrackIndex + 1;

		for (FAnimNotifyEvent* Notify : Track.Notifies)
		{
			// fix notifies indices
			Notify->TrackIndex = NewTrackIndex;
		}

		for (FAnimSyncMarker* SyncMarker : Track.SyncMarkers)
		{
			// fix notifies indices
			SyncMarker->TrackIndex = NewTrackIndex;
		}
	}

	FAnimNotifyTrack NewAnimNotifyTrack;
	NewAnimNotifyTrack.TrackName = NovaStaticFunction::GetNewTrackName(AnimSequence);
	NewAnimNotifyTrack.TrackColor = FLinearColor::White;

	AnimSequence->AnimNotifyTracks.Insert(NewAnimNotifyTrack, InTrackIndexToInsert);

	// Request a rename on rebuild
	PendingRenameTrackIndex = InTrackIndexToInsert;

	// ** 触发刷新
	auto TrackRefreshDB = GetDataBindingSP(IActImageTrackBase, "ActImageTrack/Refresh");
	TSharedPtr<FActImageTrackNotify> ActImageTrackNotify = SharedThis(this);
	TrackRefreshDB->SetData(ActImageTrackNotify);
}

void FActImageTrackNotify::RemoveTrack(int32 InTrackIndexToRemove)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	UAnimSequenceBase* AnimSequence = *(DB->GetData());

	if (AnimSequence->AnimNotifyTracks.IsValidIndex(InTrackIndexToRemove))
	{
		if (AnimSequence->AnimNotifyTracks[InTrackIndexToRemove].Notifies.Num() == 0)
		{
			FScopedTransaction Transaction(LOCTEXT("RemoveNotifyTrack", "Remove Notify Track"));
			AnimSequence->Modify();

			// before remove, make sure everything behind is fixed
			for (int32 TrackIndex = InTrackIndexToRemove + 1; TrackIndex < AnimSequence->AnimNotifyTracks.Num(); ++TrackIndex)
			{
				FAnimNotifyTrack& Track = AnimSequence->AnimNotifyTracks[TrackIndex];
				const int32 NewTrackIndex = TrackIndex - 1;

				for (FAnimNotifyEvent* Notify : Track.Notifies)
				{
					// fix notifies indices
					Notify->TrackIndex = NewTrackIndex;
				}

				for (FAnimSyncMarker* SyncMarker : Track.SyncMarkers)
				{
					// fix notifies indices
					SyncMarker->TrackIndex = NewTrackIndex;
				}
			}

			AnimSequence->AnimNotifyTracks.RemoveAt(InTrackIndexToRemove);

			// ** 触发刷新
			auto TrackRefreshDB = GetDataBindingSP(IActImageTrackBase, "ActImageTrack/Refresh");
			TSharedPtr<FActImageTrackNotify> ActImageTrackNotify = SharedThis(this);
			TrackRefreshDB->SetData(ActImageTrackNotify);
		}
	}
}

EActiveTimerReturnType FActImageTrackNotify::PendingRenameTimer(double InCurrentTime,
                                                                float InDeltaTime,
                                                                TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock)
{
	if (InInlineEditableTextBlock.IsValid())
	{
		InInlineEditableTextBlock.Pin()->EnterEditingMode();
	}

	PendingRenameTrackIndex = INDEX_NONE;

	return EActiveTimerReturnType::Stop;
}

#undef LOCTEXT_NAMESPACE
