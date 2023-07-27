// Fill out your copyright notice in the Description page of Project Settings.

#include "CEnemy.h"
#include "CEnemyAnimInstance.h"

UCEnemyAnimInstance::UCEnemyAnimInstance()
{
	Speed = 0.f;
	Direction = 0.f;

	// АјАн
	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		AttackMtg(TEXT("AnimMontage'/Game/Enemy/Animation/PrimaryAttack_RA_Montage.PrimaryAttack_RA_Montage'"));
	if (AttackMtg.Succeeded())
	{
		AttackMontage = AttackMtg.Object;
	}
}

void UCEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (IsValid(TryGetPawnOwner()))
	{
		MyEnemy = Cast<ACEnemy>(TryGetPawnOwner());
	}
}

void UCEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!IsValid(MyEnemy)) return;
	Speed = MyEnemy->GetVelocity().Size2D();
	Direction = CalculateDirection(MyEnemy->GetVelocity(), MyEnemy->GetControlRotation());
}

void UCEnemyAnimInstance::PlayAttackAnim()
{
	Montage_Play(AttackMontage);
}

void UCEnemyAnimInstance::AnimNotify_Damage()
{
	MyEnemy->SendDamage();
}
