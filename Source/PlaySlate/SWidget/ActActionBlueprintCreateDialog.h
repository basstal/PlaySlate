#pragma once
#include "ClassViewerFilter.h"
#include "PlaySlate/ActActionBlueprintFactory.h"


/**
* 提供ActAction选择父类蓝图的对话框
*/
class SActActionBlueprintCreateDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionBlueprintCreateDialog)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply OKClicked();
	void CloseDialog(bool bWasPicked = false);
	FReply CancelClicked();
	void MakeParentClassPicker();
	void OnClassPicked(UClass* ChosenClass);
	bool ConfigureProperties(TWeakObjectPtr<UActActionBlueprintFactory> InActionFactory);

protected:
	/**
	* true当点击ok时
	*/
	bool bOKClicked = false;

	/**
	* 当前选择的继承类
	*/
	TWeakObjectPtr<UClass> ParentClass;

	/**
	* 当前选择的继承类的UI container
	*/
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/**
	* 当前弹出的对话框所在的window
	*/
	TWeakPtr<SWindow> PickerWindow;

	/**
	* 对应的工厂类型
	*/
	TWeakObjectPtr<UActActionBlueprintFactory> ActionFactory;

	class FActionBlueprintParentFilter : public IClassViewerFilter
	{
	public:
		TSet<const UClass*> AllowedChildrenOfClasses;

		FActionBlueprintParentFilter()
		{
		}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
		{
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
		{
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
		}
	};
};
