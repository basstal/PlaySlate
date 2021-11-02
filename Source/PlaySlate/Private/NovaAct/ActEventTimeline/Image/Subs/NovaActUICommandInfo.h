#pragma once

class FNovaActUICommandInfo : public TCommands<FNovaActUICommandInfo>
{
public:
	FNovaActUICommandInfo();
		

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> EditSelectedCurves;

	TSharedPtr<FUICommandInfo> RemoveSelectedCurves;

	TSharedPtr<FUICommandInfo> AddNotifyTrack;

	TSharedPtr<FUICommandInfo> InsertNotifyTrack;

	TSharedPtr<FUICommandInfo> RemoveNotifyTrack;

	TSharedPtr<FUICommandInfo> AddCurve;

	TSharedPtr<FUICommandInfo> EditCurve;

	TSharedPtr<FUICommandInfo> ShowCurveKeys;

	TSharedPtr<FUICommandInfo> AddMetadata;

	TSharedPtr<FUICommandInfo> ConvertCurveToMetaData;

	TSharedPtr<FUICommandInfo> ConvertMetaDataToCurve;

	TSharedPtr<FUICommandInfo> RemoveCurve;

	TSharedPtr<FUICommandInfo> RemoveAllCurves;

	TSharedPtr<FUICommandInfo> DisplaySeconds;

	TSharedPtr<FUICommandInfo> DisplayFrames;

	TSharedPtr<FUICommandInfo> DisplayPercentage;

	TSharedPtr<FUICommandInfo> DisplaySecondaryFormat;

	TSharedPtr<FUICommandInfo> SnapToFrames;

	TSharedPtr<FUICommandInfo> SnapToNotifies;

	TSharedPtr<FUICommandInfo> SnapToMontageSections;

	TSharedPtr<FUICommandInfo> SnapToCompositeSegments;
};

class FNovaActNotifiesPanelCommands : public TCommands<FNovaActNotifiesPanelCommands>
{
public:
	FNovaActNotifiesPanelCommands();

	TSharedPtr<FUICommandInfo> DeleteNotify;

	TSharedPtr<FUICommandInfo> CopyNotifies;

	TSharedPtr<FUICommandInfo> PasteNotifies;

	virtual void RegisterCommands() override;
};