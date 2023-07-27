// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UCEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UCEnemyAnimInstance();
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", Meta = (AllowPrivateAccess = true))
		float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", Meta = (AllowPrivateAccess = true))
		float Direction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", Meta = (AllowPrivateAccess = true))
		class ACEnemy* MyEnemy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", Meta = (AllowPrivateAccess = true))
		class UAnimMontage* AttackMontage;
public:
	void PlayAttackAnim();
private:
	UFUNCTION()
		void AnimNotify_Damage();
};
