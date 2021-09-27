#pragma once

#include "ActActionSequence.generated.h"

UCLASS()
class UActActionSequence : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UAnimInstance* EditAnimInstance;

	UPROPERTY()
	UAnimMontage* EditAnimMontage;
	
	/** The resolution at which all frame numbers within this data are stored */
	UPROPERTY()
	FFrameRate TickResolution;

};
