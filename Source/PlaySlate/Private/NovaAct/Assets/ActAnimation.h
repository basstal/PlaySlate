#pragma once

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
};
