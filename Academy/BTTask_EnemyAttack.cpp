// Fill out your copyright notice in the Description page of Project Settings.
#include "BehaviorTree/BlackboardComponent.h"

#include "CEnemyAIController.h"
#include "CEnemyAnimInstance.h"
#include "CEnemy.h"

#include "BTTask_EnemyAttack.h"

UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = TEXT("Attack");
	bNotifyTick = true;//틱함수 사용
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (OwnerComp.GetAIOwner() == nullptr) return EBTNodeResult::Failed;
	if (OwnerComp.GetAIOwner()->GetPawn() == nullptr)
		return EBTNodeResult::Failed;

	ACEnemy* Enemy = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UCEnemyAnimInstance* Anim = Cast<UCEnemyAnimInstance>(Enemy->GetMesh()->GetAnimInstance());

	if (Anim == nullptr) return EBTNodeResult::Failed;
	ACEnemyAIController* AIController = Cast<ACEnemyAIController>(Enemy->GetController());
	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	FVector Direction;

	if (BB->GetValueAsBool(TEXT("isFindPlayer")))
	{
		AActor* Player = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
		
		Direction = Player->GetActorLocation() - Enemy->GetActorLocation();
		Direction.Normalize();

		FRotator Rotation = Direction.Rotation();
		Rotation.Pitch = 0.f;
		Enemy->SetActorRotation(Rotation);
	}
	else if (BB->GetValueAsBool(TEXT("isFindDefenceActor")))
	{
		AActor* Defence = Cast<AActor>(BB->GetValueAsObject(TEXT("DefenceActor")));
		Direction = Defence->GetActorLocation() - Enemy->GetActorLocation();
		Direction.Normalize();
		
		FRotator Rotation = Direction.Rotation();
		Rotation.Pitch = 0.f;
		Enemy->SetActorRotation(Rotation);
	}
	Anim->PlayAttackAnim(); //공격 몽타주 재생
	return EBTNodeResult::InProgress; //진행중 반환
}

void UBTTask_EnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (OwnerComp.GetAIOwner() == nullptr) return;
	if (OwnerComp.GetAIOwner()->GetPawn() == nullptr) return;

	ACEnemy* Enemy = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UCEnemyAnimInstance* Anim = Cast<UCEnemyAnimInstance>(Enemy->GetMesh()->GetAnimInstance());

	if (Anim == nullptr) return;
	if (Anim->Montage_GetIsStopped(Enemy->GetCurrentMontage()))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); //성공반환
	}
}
