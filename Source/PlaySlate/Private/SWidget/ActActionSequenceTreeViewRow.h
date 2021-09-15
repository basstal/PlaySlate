// #pragma once
//
// #include "SWidget/ActActionSequenceTreeView.h"
// #include "Editor/ActActionSequenceDisplayNode.h"
//
// class SActActionSequenceTreeViewRow : public SMultiColumnTableRow<TSharedRef<FActActionSequenceDisplayNode>>
// {
// 	DECLARE_DELEGATE_RetVal_ThreeParams(TSharedRef<SWidget>, FOnGenerateWidgetForColumn, const TSharedRef<FActActionSequenceDisplayNode>&, const FName&, const TSharedRef<SActActionSequenceTreeViewRow>&);
// public:
// 	SLATE_BEGIN_ARGS(SActActionSequenceTreeViewRow){}
// 		/** FIX:Delegate to invoke to create a new column for this row */
// 		SLATE_EVENT(FOnGenerateWidgetForColumn, OnGenerateWidgetForColumn)
// 	SLATE_END_ARGS()
//
// 	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<FActActionSequenceDisplayNode>& InNode);
// };
