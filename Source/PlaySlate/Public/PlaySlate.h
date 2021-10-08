// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Utils/ActActionSequenceUtil.h"

class IAssetTypeActions;

DECLARE_LOG_CATEGORY_EXTERN(LogActAction, Log, All)

class FPlaySlateModule : public IModuleInterface
{
public:
	
	//~Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End IModuleInterface interface

	/**
	 * @param InAssetTypeActions 待注册的IAssetTypeActions资源类型
	 */
	void RegisterAssetTypeActions(TSharedRef<IAssetTypeActions> InAssetTypeActions);
	
	/** 
	* Register a delegate that will create an editor for a track in each sequence.
	*
	* @param InOnCreateTrackEditor	Delegate to register.
	*/
	void RegisterTrackEditor(ActActionSequence::OnCreateTrackEditorDelegate InOnCreateTrackEditor);
	/** 
	* Unregisters a previously registered delegate for creating a track editor
	*
	* @param InHandle	Handle to the delegate to unregister
	*/
	void UnRegisterTrackEditor(FDelegateHandle InHandle);

protected:
	
	/**
	 * List of auto-key handler delegates sequencers will execute when they are created
	 */
	TArray<ActActionSequence::OnCreateTrackEditorDelegate> TrackEditorDelegates;
	/**
	* 所有已注册的资源类型，登记在这里以便在模块关闭时反注册
	*/
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;

public:
	
	TArray<ActActionSequence::OnCreateTrackEditorDelegate> GetTrackEditorDelegates() const
	{
		return TrackEditorDelegates;
	}
};
