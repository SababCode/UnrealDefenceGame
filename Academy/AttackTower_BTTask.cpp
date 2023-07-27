// Fill out your copyright notice in the Description page of Project Settings.
#include "BehaviorTree/BlackboardComponent.h"
#include "CEnemy.h"
#include "CEnemyAIController.h"

#include "ATowerAnimInstance.h"
#include "CDefenceTower.h"
#include "AAttackDefenceAI.h"
#include "AttackTower_BTTask.h"

UAttackTower_BTTask::UAttackTower_BTTask()
{
	NodeName = TEXT("AttackTower");
	bNotifyTick = true;
}
EBTNodeResult::Type UAttackTower_BTTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (OwnerComp.GetAIOwner() == nullptr) return EBTNodeResult::Failed;
	if (OwnerComp.GetAIOwner()->GetPawn() == nullptr) return EBTNodeResult::Failed;
	ACDefenceTower* AttackTower = Cast<ACDefenceTower>(OwnerComp.GetAIOwner()->GetPawn());
	UATowerAnimInstance* Anim = Cast<UATowerAnimInstance>(AttackTower->GetMesh()->GetAnimInstance());
	if (Anim == nullptr) return EBTNodeResult::Failed;

	AAAttackDefenceAI* TowerAI = Cast<AAAttackDefenceAI>(AttackTower->GetController());
	UBlackboardComponent* TowerBB = TowerAI->GetBlackboardComponent();
	FVector Direction;

	if (TowerBB->GetValueAsBool(TEXT("isFindEnemy")))
	{
		AActor* Enemy = Cast<AActor>(TowerBB->GetValueAsObject(TEXT("EnemyActor")));
		if (Enemy == nullptr) return EBTNodeResult::Failed;

		//적을 바라보게 회전
		Direction = Enemy->GetActorLocation() - AttackTower->GetActorLocation();
		Direction.Normalize();
		AttackTower->SetActorRotation(Direction.Rotation());
	}
		Anim->PlayAttackAnim();//공격 몽타주
		return EBTNodeResult::InProgress;
}

void UAttackTower_BTTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (OwnerComp.GetAIOwner() == nullptr) return;
	if (OwnerComp.GetAIOwner()->GetPawn() == nullptr) return;
	
	ACDefenceTower* AttackTower = Cast<ACDefenceTower>(OwnerComp.GetAIOwner()->GetPawn());
	UATowerAnimInstance* Anim = Cast<UATowerAnimInstance>(AttackTower->GetMesh()->GetAnimInstance());
	if (Anim == nullptr) return;

	if (Anim->Montage_GetIsStopped(Anim->GetCurrentActiveMontage()))
	{
		AttackTower->Shot();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); //성공반환
	}
}
