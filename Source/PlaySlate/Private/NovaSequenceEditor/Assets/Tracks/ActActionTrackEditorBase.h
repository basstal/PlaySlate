#pragma once

class FActActionSequenceController;

class FActActionTrackEditorBase : public TSharedFromThis<FActActionTrackEditorBase>
{
public:
	FActActionTrackEditorBase(const TSharedRef<FActActionSequenceController>& InSequenceController);
	virtual ~FActActionTrackEditorBase();
	
	/**
	* Returns whether a sequence is supported by this tool.
	*
	* @param InSequence The sequence that could be supported.
	* @return true if the type is supported.
	*/
	// virtual bool SupportsSequence(UActActionSequence* InSequence) const = 0;

	/**
	* Builds up the sequencer's "Add Track" menu.
	*
	* @param MenuBuilder The menu builder to change.
	*/
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) = 0;

protected:
	TWeakPtr<FActActionSequenceController> SequenceController;
};
