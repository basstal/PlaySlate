#include "ActActionSequenceMain.h"

#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "SEditorHeaderButton.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

SActActionSequenceMain::SActActionSequenceMain()
{
	ColumnFillCoefficients[0] = 0.4f;
	ColumnFillCoefficients[1] = 0.6f;
}

void SActActionSequenceMain::Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceController> InSequenceController)
{
	SequenceController = InSequenceController;
	
	TAttribute<float> FillCoefficient_0, FillCoefficient_1;
	FillCoefficient_0.Bind(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceMain::GetColumnFillCoefficient, 0));
	FillCoefficient_1.Bind(TAttribute<float>::FGetter::CreateSP(this, &SActActionSequenceMain::GetColumnFillCoefficient, 1));


	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SSplitter).Orientation(Orient_Horizontal)
			+ SSplitter::Slot().Value(0.9f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SGridPanel).FillRow(2, 1.0f).FillColumn(0, FillCoefficient_0).FillColumn(1, FillCoefficient_1)
					// ** TODO:暂缺工具栏
	
					+ SGridPanel::Slot(0, 1)
					[
						SNew(SBorder).BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSpacer)
						]
					]
	
					// ** FIX:outliner search box 搜索框？
					+ SGridPanel::Slot(0, 1, SGridPanel::Layer(10))
					[
						SNew(SBorder).BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(0.6f, 0.3f)).Clipping(EWidgetClipping::ClipToBounds)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FMargin(0, 0, 0.3f, 0))
							[
								MakeAddButton()
							]
	
							+ SHorizontalBox::Slot().VAlign(VAlign_Center)
							[
								SAssignNew(SearchBox, SSearchBox).HintText(LOCTEXT("SearchNodesHint", "Search Tracks")).OnTextChanged(this, &SActActionSequenceMain::OnOutlinerSearchChanged)
							]
	
						]
					]
	
				]
	
			]
		]
	];
}

int32 SActActionSequenceMain::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void SActActionSequenceMain::OnOutlinerSearchChanged(const FText& Filter)
{
	TSharedPtr<FActActionSequenceController> Sequence = SequenceController.Pin();
	if (Sequence.IsValid())
	{
		const FString FilterString = Filter.ToString();
		// Sequence->GetNodeTree()->SetFilterNodes(FilterString);
		// TreeView->Refresh();
	}
}

TSharedRef<SWidget> SActActionSequenceMain::MakeAddButton()
{
	return SNew(SEditorHeaderButton).OnGetMenuContent(this, &SActActionSequenceMain::MakeAddMenu).Icon(FAppStyle::Get().GetBrush("Icons.Plus")).Text(LOCTEXT("Track", "Track")).IsEnabled_Lambda([=]()
	{
		// return SequencePtr.Pin().IsValid() && !SequencePtr.Pin()->IsReadOnly();
		return SequenceController.Pin().IsValid();
	});
}

TSharedRef<SWidget> SActActionSequenceMain::MakeAddMenu()
{
	TSharedPtr<FExtender> Extender = FExtender::Combine(AddMenuExtenders);
	FMenuBuilder MenuBuilder(true, nullptr, Extender);
	PopulateAddMenuContext(MenuBuilder);
	return MenuBuilder.MakeWidget();
}

void SActActionSequenceMain::PopulateAddMenuContext(FMenuBuilder& MenuBuilder)
{
	// ** 让Toolkits填充该菜单
	TSharedPtr<FActActionSequenceController> Sequence = SequenceController.Pin();
	MenuBuilder.BeginSection("MainMenu");
	if (Sequence.IsValid())
	{
		OnGetAddMenuContent.ExecuteIfBound(MenuBuilder, Sequence.ToSharedRef());
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("ObjectBindings");
	if (Sequence.IsValid())
	{
		Sequence->BuildAddObjectBindingsMenu(MenuBuilder);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("AddTracks");
	if (Sequence.IsValid())
	{
		Sequence->BuildAddTrackMenu(MenuBuilder);
	}
	MenuBuilder.EndSection();
}


#undef LOCTEXT_NAMESPACE
