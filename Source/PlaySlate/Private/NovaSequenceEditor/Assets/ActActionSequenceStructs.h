#pragma once

#include "ActActionSequenceStructs.generated.h"

USTRUCT()
struct FActActionHitBoxData
{
	GENERATED_BODY()

	/**
	 * 开始帧
	 */
	UPROPERTY(EditAnywhere)
	int Start;

	/**
	 * 结束帧
	 */
	UPROPERTY(EditAnywhere)
	int End;

	/**
	 * 攻击盒数据
	 */
	UPROPERTY(EditAnywhere)
	FOrientedBox Box;

	/**
	 * 攻击盒分组
	 */
	UPROPERTY(EditAnywhere)
	FName GroupId;
};
