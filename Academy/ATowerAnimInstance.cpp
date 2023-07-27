// Fill out your copyright notice in the Description page of Project Settings.

#include "CDefenceTower.h"
#include "ATowerAnimInstance.h"

UATowerAnimInstance::UATowerAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		AttackMtg(L"AnimMontage'/Game/Tower/Animation/Archer_Attack_Montage.Archer_Attack_Montage'");
	if (AttackMtg.Succeeded())
	{
		AttackMontage = AttackMtg.Object;
	}
}
void UATowerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (IsValid(TryGetPawnOwner()))
	{
		MyTower = Cast<ACDefenceTower>(TryGetPawnOwner());
	}
}

//void UATowerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
//{
//	Super::NativeUpdateAnimation(DeltaSeconds);
//}

void UATowerAnimInstance::PlayAttackAnim()
{
	Montage_Play(AttackMontage, MyTower->GetAttackSpeed());
}

void UATowerAnimInstance::AnimNotify_Damage()
{

}
