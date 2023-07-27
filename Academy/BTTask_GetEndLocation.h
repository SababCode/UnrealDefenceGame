// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetEndLocation.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UBTTask_GetEndLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_GetEndLocation();
protected:
	EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "Point", meta = (AllowPrivateAccess = true))
		int32 PointCount;
	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "Point", meta = (AllowPrivateAccess = true))
		bool isArrive;
	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "Point", meta = (AllowPrivateAccess = true))
		FVector PointVector;
};
