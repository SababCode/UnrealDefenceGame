// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "AttackTower_BTTask.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UAttackTower_BTTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UAttackTower_BTTask();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
