#include "ActActionSequenceSplitterOverlay.h"

void SActActionSequenceSplitterOverlay::Construct(const FArguments& InArgs)
{
	SetVisibility(EVisibility::SelfHitTestInvisible);

	// ** 这种语法将参数作为SNew的第一个参数传入，因为在SNew中的参数将作为除FArguments以外的第一个参数传入，并且RequiredArgs::MakeRequiredArgs也无法直接赋值结构体
	Splitter = SNew(SSplitter) = InArgs;
	Splitter->SetVisibility(EVisibility::HitTestInvisible);
	AddSlot()
	[
		Splitter.ToSharedRef()
	];

	for (int32 Index = 0; Index < Splitter->GetChildren()->Num() - 1; ++Index)
	{
		AddSlot()
			.Padding(TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateLambda([=]()
			{
				if (SlotPadding.IsValidIndex(Index))
				{
					return SlotPadding[Index];
				}

				return FMargin(0.0f);
			})))
			[
				SNew(SBox)
				.Visibility(EVisibility::Visible)
			];
	}
}

void SActActionSequenceSplitterOverlay::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	FArrangedChildren SplitterChildren(ArrangedChildren.GetFilter());
	Splitter->ArrangeChildren(AllottedGeometry, SplitterChildren);

	SlotPadding.Reset();

	for (int32 Index = 0; Index < SplitterChildren.Num() - 1; ++Index)
	{
		const FGeometry& ThisGeometry = SplitterChildren[Index].Geometry;
		const FGeometry& NextGeometry = SplitterChildren[Index + 1].Geometry;

		if (Splitter->GetOrientation() == EOrientation::Orient_Horizontal)
		{
			SlotPadding.Add(FMargin(
					ThisGeometry.Position.X + ThisGeometry.GetLocalSize().X,
					0,
					AllottedGeometry.Size.X - NextGeometry.Position.X,
					0)
			);
		}
		else
		{
			SlotPadding.Add(FMargin(
					0,
					ThisGeometry.Position.Y + ThisGeometry.GetLocalSize().Y,
					0,
					AllottedGeometry.Size.Y - NextGeometry.Position.Y)
			);
		}
	}

	SOverlay::OnArrangeChildren(AllottedGeometry, ArrangedChildren);
}

FCursorReply SActActionSequenceSplitterOverlay::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	return Splitter->OnCursorQuery(MyGeometry, CursorEvent);
}

FReply SActActionSequenceSplitterOverlay::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = Splitter->OnMouseButtonDown(MyGeometry, MouseEvent);
	if (Reply.GetMouseCaptor().IsValid())
	{
		// Set us to be the mouse captor so we can forward events properly
		Reply.CaptureMouse(SharedThis(this));
		SetVisibility(EVisibility::Visible);
	}
	return Reply;
}

void SActActionSequenceSplitterOverlay::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	SetVisibility(EVisibility::SelfHitTestInvisible);
	SOverlay::OnMouseCaptureLost(CaptureLostEvent);
}

FReply SActActionSequenceSplitterOverlay::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = Splitter->OnMouseButtonUp(MyGeometry, MouseEvent);
	if (Reply.ShouldReleaseMouse())
	{
		SetVisibility(EVisibility::SelfHitTestInvisible);
	}
	return Reply;
}

FReply SActActionSequenceSplitterOverlay::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return Splitter->OnMouseMove(MyGeometry, MouseEvent);
}

void SActActionSequenceSplitterOverlay::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	Splitter->OnMouseLeave(MouseEvent);
}
