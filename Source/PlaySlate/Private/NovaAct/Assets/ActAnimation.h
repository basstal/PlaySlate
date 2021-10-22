#pragma once

#include "ActActionSequenceStructs.h"

#include "ActAnimation.generated.h"

UCLASS()
class UActAnimation : public UObject
{
	GENERATED_BODY()

public:
	UActAnimation();

	UPROPERTY(EditAnywhere)
	UAnimBlueprint* AnimBlueprint;

	UPROPERTY(EditAnywhere)
	UAnimSequence* AnimSequence;

	/** The resolution at which all frame numbers within this data are stored */
	UPROPERTY(EditAnywhere)
	FFrameRate TickResolution;

	UPROPERTY(EditAnywhere)
	TArray<FActActionHitBoxData> ActActionHitBoxes;
};
