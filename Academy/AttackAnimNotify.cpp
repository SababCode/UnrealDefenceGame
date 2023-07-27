// Fill out your copyright notice in the Description page of Project Settings.

#include "CPlayer.h"
#include "AttackAnimNotify.h"

FString UAttackAnimNotify::GetNotifyName_Implementation() const
{
	return TEXT("NextAttack");
}
 
void UAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (MeshComp == nullptr) return;
	ACPlayer* MyPlayer = Cast<ACPlayer>(MeshComp->GetOwner());
	if (MyPlayer == nullptr) return;
	MyPlayer->NextAttack();
}
