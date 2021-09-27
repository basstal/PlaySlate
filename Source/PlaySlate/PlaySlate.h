// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utils/ActActionSequenceUtil.h"

DECLARE_LOG_CATEGORY_EXTERN(LogActAction, Log, All)

class IAssetTypeActions;

class FPlaySlateModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** 
	* Registers a delegate that will create an editor for a track in each sequence.
	*
	* @param InOnCreateTrackEditor	Delegate to register.
	* @return A handle to the newly-added delegate.
	*/
	FDelegateHandle RegisterTrackEditor(ActActionSequence::OnCreateTrackEditorDelegate InOnCreateTrackEditor, TArrayView<ActActionSequence::FActActionAnimatedPropertyKey> AnimatedPropertyTypes = TArrayView<ActActionSequence::FActActionAnimatedPropertyKey>());
	/** 
	* Unregisters a previously registered delegate for creating a track editor
	*
	* @param InHandle	Handle to the delegate to unregister
	*/
	void UnRegisterTrackEditor( FDelegateHandle InHandle );

protected:
	TSet<ActActionSequence::FActActionAnimatedPropertyKey> PropertyAnimators;

	/** List of auto-key handler delegates sequencers will execute when they are created */
	TArray< ActActionSequence::OnCreateTrackEditorDelegate > TrackEditorDelegates;
public:
	TArray<ActActionSequence::OnCreateTrackEditorDelegate> GetTrackEditorDelegates() const
	{
		return TrackEditorDelegates;
	}

protected:
	/**
	* 所有已注册的资源类型，登记在这里以便在模块关闭时反注册
	*/
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;

	/** Map of all track editor factories to property types that they have registered to animated */
	TArray<ActActionSequence::FActActionAnimatedTypeCache> AnimatedTypeCache;
};
