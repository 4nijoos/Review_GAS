// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ActionAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class REVIEW_GAS_API UActionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	//초기화용
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;	//이벤트그래프의 

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;
	
};
