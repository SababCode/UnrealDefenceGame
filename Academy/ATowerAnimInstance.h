// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ATowerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UATowerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UATowerAnimInstance();
protected:
	virtual void NativeInitializeAnimation() override;
	//virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower", Meta = (AllowPrivateAccess = true))
		class ACDefenceTower* MyTower;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower", Meta = (AllowPrivateAccess = true))
		class UAnimMontage* AttackMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower", Meta = (AllowPrivateAccess = true))
		bool isAttackTower;
public:
	void PlayAttackAnim();
private:
	UFUNCTION()
		void AnimNotify_Damage();
};
