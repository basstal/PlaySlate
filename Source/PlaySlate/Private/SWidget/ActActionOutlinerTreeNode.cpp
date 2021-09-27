#include "ActActionOutlinerTreeNode.h"

#include "ActActionSequenceTreeView.h"
#include "Editor/ActActionSequenceController.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

void SActActionOutlinerTreeNode::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceDisplayNode>& Node, const TSharedRef<SActActionSequenceTreeViewRow>& InTableRow)
{
	DisplayNode = Node;
	bIsOuterTopLevelNode = !Node->GetParent().IsValid();
	bIsInnerTopLevelNode = Node->GetType() != ActActionSequence::ESequenceNodeType::Folder && Node->GetParent().IsValid() && Node->GetParent()->GetType() == ActActionSequence::ESequenceNodeType::Folder;

	if (bIsOuterTopLevelNode)
	{
		// ExpandedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );
		// CollapsedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Collapsed" );
	}
	else
	{
		// ExpandedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.DefaultBorder" );
		// CollapsedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.DefaultBorder" );
	}

	FMargin InnerNodePadding;
	if ( bIsInnerTopLevelNode )
	{
		// InnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );
		InnerNodePadding = FMargin(0.f, 1.f);
	}
	else
	{
		// InnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TransparentBorder" );
		InnerNodePadding = FMargin(0.f);
	}

	TableRowStyle = &FEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");

	EditableLabel = SNew(SInlineEditableTextBlock)
		.IsReadOnly(this, &SActActionOutlinerTreeNode::IsNodeLabelReadOnly)
		.Font(this, &SActActionOutlinerTreeNode::GetDisplayNameFont)
		.ColorAndOpacity(this, &SActActionOutlinerTreeNode::GetDisplayNameColor)
		.OnVerifyTextChanged(this, &SActActionOutlinerTreeNode::VerifyNodeTextChanged)
		.OnTextCommitted(this, &SActActionOutlinerTreeNode::HandleNodeLabelTextCommitted)
		// .Text(this, &SActActionOutlinerTreeNode::GetDisplayName)
		// .ToolTipText(this, &SActActionOutlinerTreeNode::GetDisplayNameToolTipText)
		.Clipping(EWidgetClipping::ClipToBounds)
		.IsSelected(FIsSelected::CreateSP(InTableRow, &SActActionSequenceTreeViewRow::IsSelectedExclusively));

	TSharedRef<SWidget> LabelContent = EditableLabel.ToSharedRef();

	// if (TSharedPtr<SWidget> AdditionalLabelContent = Node->GetAdditionalOutlinerLabel())
	// {
	// 	LabelContent = SNew(SHorizontalBox)
	//
	// 		+ SHorizontalBox::Slot()
	// 		  .AutoWidth()
	// 		  .Padding(FMargin(0.f, 0.f, 5.f, 0.f))
	// 		[
	// 			LabelContent
	// 		]
	//
	// 		+ SHorizontalBox::Slot()
	// 		[
	// 			AdditionalLabelContent.ToSharedRef()
	// 		];
	//
	// 	LabelContent->SetClipping(EWidgetClipping::ClipToBounds);
	// }

	// Node->OnRenameRequested().AddRaw(this, &SActActionOutlinerTreeNode::EnterRenameMode);

	auto NodeHeight = [=]() -> FOptionalSize { return DisplayNode->GetNodeHeight(); };

	// ForegroundColor.Bind(this, &SActActionOutlinerTreeNode::GetForegroundBasedOnSelection);

	TSharedRef<SWidget> FinalWidget =
		SNew(SBorder)
		.VAlign(VAlign_Center)
		// .BorderImage(this, &SActActionOutlinerTreeNode::GetNodeBorderImage)
		// .BorderBackgroundColor(this, &SActActionOutlinerTreeNode::GetNodeBackgroundTint)
		// .Padding(FMargin(0, Node->GetNodePadding().Combined() / 2))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.HeightOverride_Lambda(NodeHeight)
				.Padding(FMargin(5.0f, 0.0f))
				[
					SNew(SHorizontalBox)

					// Expand track lanes button
					+ SHorizontalBox::Slot()
					  .Padding(FMargin(2.f, 0.f, 2.f, 0.f))
					  .VAlign(VAlign_Center)
					  .AutoWidth()
					[
						SNew(SExpanderArrow, InTableRow)
						.IndentAmount(10.0f)
					]

					+ SHorizontalBox::Slot()
					.Padding(InnerNodePadding)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("Sequencer.AnimationOutliner.TopLevelBorder_Collapsed"))
						// .BorderBackgroundColor(this, &SActActionOutlinerTreeNode::GetNodeInnerBackgroundTint)
						.Padding(FMargin(0))
						[
							SNew(SHorizontalBox)

							// Icon
							+ SHorizontalBox::Slot()
							  .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
							  .VAlign(VAlign_Center)
							  .AutoWidth()
							[
								SNew(SOverlay)

								+ SOverlay::Slot()
								[
									SNew(SImage)
									.Image(InArgs._IconBrush)
									.ColorAndOpacity(InArgs._IconColor)
								]

								+ SOverlay::Slot()
								  .VAlign(VAlign_Top)
								  .HAlign(HAlign_Right)
								[
									SNew(SImage)
									.Image(InArgs._IconOverlayBrush)
								]

								+ SOverlay::Slot()
								[
									SNew(SSpacer)
									.Visibility(EVisibility::Visible)
									.ToolTipText(InArgs._IconToolTipText)
								]
							]

							// Label Slot
							+ SHorizontalBox::Slot()
							  .VAlign(VAlign_Center)
							  .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
							[
								LabelContent
							]

							// Arbitrary customization slot
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								InArgs._CustomContent.Widget
							]
						]
					]
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.ContentPadding(0)
				.VAlign(VAlign_Fill)
				.IsFocusable(false) // Intentionally false so that it's easier to tab to the next numeric input
				.IsEnabled(!DisplayNode->GetSequence()->IsReadOnly())
				.ButtonStyle(FEditorStyle::Get(), "Sequencer.AnimationOutliner.ColorStrip")
				// .OnClicked(this, &SActActionOutlinerTreeNode::OnSetTrackColor)
				.Content()
				[
					SNew(SBox)
					.WidthOverride(6.f)
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush("WhiteBrush"))
						// .ColorAndOpacity(this, &SActActionOutlinerTreeNode::GetTrackColorTint)
					]
				]
			]
		];

	ChildSlot
	[
		FinalWidget
	];
}

TSharedPtr<FActActionSequenceDisplayNode> SActActionOutlinerTreeNode::GetParent() const
{
	TSharedPtr<FActActionSequenceDisplayNode> Pinned = ParentNode.Pin();
	return (Pinned && Pinned->GetType() != ActActionSequence::ESequenceNodeType::Root) ? Pinned : nullptr;
}

bool SActActionOutlinerTreeNode::IsNodeLabelReadOnly() const
{
	return DisplayNode->GetSequence()->IsReadOnly() || !DisplayNode->CanRenameNode();
}

FSlateFontInfo SActActionOutlinerTreeNode::GetDisplayNameFont() const
{
	return DisplayNode->GetDisplayNameFont();
}

FSlateColor SActActionOutlinerTreeNode::GetDisplayNameColor() const
{
	return DisplayNode->GetDisplayNameColor();
}

bool SActActionOutlinerTreeNode::VerifyNodeTextChanged(const FText& NewLabel, FText& OutErrorMessage)
{
	return DisplayNode->ValidateDisplayName(NewLabel, OutErrorMessage);
}

void SActActionOutlinerTreeNode::HandleNodeLabelTextCommitted(const FText& NewLabel, ETextCommit::Type CommitType)
{
	DisplayNode->SetDisplayName(NewLabel);
}
