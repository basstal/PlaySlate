#pragma once

#include "ActActionSequence.generated.h"

UCLASS()
class UActActionSequence : public UObject
{
	GENERATED_BODY()

public:
	UActActionSequence();

	UPROPERTY()
	UAnimBlueprint* EditAnimBlueprint;

	UPROPERTY()
	UAnimMontage* EditAnimMontage;

	/** The resolution at which all frame numbers within this data are stored */
	UPROPERTY()
	FFrameRate TickResolution;

	UPROPERTY()
	FFrameRate DisplayRate;

	/**
	 * 动画播放的帧区间
	 */
	TRange<FFrameNumber> PlaybackRange;

	/** User-defined selection range. */
	TRange<FFrameNumber> SelectionRange;
};
