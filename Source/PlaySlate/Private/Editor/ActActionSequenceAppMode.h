// #pragma once
// #include "ActActionSequenceEditor.h"
// #include "WorkflowOrientedApp/ApplicationMode.h"
// #include "WorkflowOrientedApp/WorkflowTabManager.h"
//
// /**
//  * 对ActActionSequenceEditor的包装，把多个Editor集合成一个Toolkit，切换不同的Mode？？
//  * 注册Tab工厂以创建Editor需要的Tab？？
//  */
// class FActActionSequenceEditorMode : public FApplicationMode
// {
// public:
// 	FActActionSequenceEditorMode(const TSharedRef<FActActionSequenceEditor>& InSequenceEditor);
//
// 	//~Begin FApplicationMode interface
// 	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
// 	//~End FApplicationMode interface
//
// protected:
// 	/**
// 	 * Set of spawnable tabs
// 	 */
// 	FWorkflowAllowedTabSet TabFactories;
// 	/**
// 	 * Mode所属的Editor
// 	 */
// 	TWeakPtr<FActActionSequenceEditor> SequenceEditor;
// };
