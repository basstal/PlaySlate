#pragma once

#include "Editor/ActActionSequenceController.h"
#include "Widgets/SCompoundWidget.h"

class SActActionSequenceTreeView;
DECLARE_DELEGATE_TwoParams(FActActionOnGetAddMenuContent, FMenuBuilder&, TSharedRef<FActActionSequenceController>);

/**
 * Sequence的主要UI
 */
class SActActionSequenceMain : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceMain)
		{
		}

	SLATE_END_ARGS()

	SActActionSequenceMain();

	void Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceController> InSequenceController);

	//~Begin SCompoundWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SCompoundWidget interface

protected:
	/**
	 * 搜素框内文本改变的回调
	 */
	void OnOutlinerSearchChanged(const FText& Filter);
	/**
	 * 搜素和Track过滤功能
	 */
	TSharedPtr<SSearchBox> SearchBox;
	/**
	 * 当前View的Controller,转发所有控制Model的行为
	 */
	TWeakPtr<FActActionSequenceController> SequenceController;
	/**
	 * 用做左侧显示outliner和track区域的sequence tree view 
	 */
	TSharedPtr<SActActionSequenceTreeView> TreeView;


	/**
	 * FIX:搜索栏添加按钮？
	 */
	TSharedRef<SWidget> MakeAddButton();

	/**
	 * 添加菜单的内容
	 */
	TArray<TSharedPtr<FExtender>> AddMenuExtenders;
	/**
	 * 构造添加菜单的按钮SWidget
	 */
	TSharedRef<SWidget> MakeAddMenu();
	/**
	 * 添加菜单填充内容的事件
	 */
	FActActionOnGetAddMenuContent OnGetAddMenuContent;
	/**
	 * 往添加菜单中填充其内容
	 */
	void PopulateAddMenuContext(FMenuBuilder& MenuBuilder);


	/** 整个Sequence轨道的左侧和右侧分别占比 */
	float ColumnFillCoefficients[2];
	/**
	 * @return 0 - 左侧占比 1 - 右侧占比
	 */
	float GetColumnFillCoefficient(int32 ColumnIndex) const
	{
		return ColumnFillCoefficients[ColumnIndex];
	}
};
