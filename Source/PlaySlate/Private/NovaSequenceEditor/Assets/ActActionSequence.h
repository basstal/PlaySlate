#pragma once

#include "ActActionSequence.generated.h"

UCLASS()
class UActActionSequence : public UObject
{
	GENERATED_BODY()

public:
	UActActionSequence();

	UPROPERTY(EditAnywhere)
	UAnimBlueprint* EditAnimBlueprint;

	UPROPERTY(EditAnywhere)
	UAnimMontage* EditAnimMontage;

	/** The resolution at which all frame numbers within this data are stored */
	UPROPERTY(EditAnywhere)
	FFrameRate TickResolution;

	UPROPERTY(EditAnywhere)
	FFrameRate DisplayRate;
};
