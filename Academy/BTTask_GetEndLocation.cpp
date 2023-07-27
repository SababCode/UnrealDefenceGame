// Fill out your copyright notice in the Description page of Project Settings.

#include "CEnemyAIController.h"
#include "CEnemy.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "BTTask_GetEndLocation.h"

UBTTask_GetEndLocation::UBTTask_GetEndLocation()
{
    NodeName = TEXT("GetEndLocation");
    isArrive = false;
    PointCount = 0;
    PointVector = FVector(-1173.f, 1529.0f, -449.f);
}

EBTNodeResult::Type UBTTask_GetEndLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    ACEnemyAIController* EnemyController = Cast<ACEnemyAIController>(OwnerComp.GetAIOwner());
    if (EnemyController == nullptr) return EBTNodeResult::Failed;
    APawn* Enemy = EnemyController->GetPawn();
    if (Enemy == nullptr) return EBTNodeResult::Failed;

    EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("EndLocation"), PointVector);

    return EBTNodeResult::Succeeded; //성공반환
}
