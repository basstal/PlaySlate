#pragma once

#include "ActActionSequence.generated.h"

UCLASS()
class UActActionSequence : public UObject
{
	GENERATED_BODY()

public:
	UActActionSequence();
	
	UPROPERTY()
	UAnimInstance* EditAnimInstance;

	UPROPERTY()
	UAnimMontage* EditAnimMontage;

	/** The resolution at which all frame numbers within this data are stored */
	UPROPERTY()
	FFrameRate TickResolution;

	UPROPERTY()
	FFrameRate DisplayRate;
	
	/** User-defined playback range for this movie scene. Must be a finite range. Relative to this movie-scene's 0-time origin. */
	TRange<FFrameNumber> PlaybackRange;

	/** User-defined selection range. */
	TRange<FFrameNumber> SelectionRange;
};
