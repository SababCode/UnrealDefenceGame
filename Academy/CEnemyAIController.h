// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CEnemy.h"

#include "Perception/AIPerceptionTypes.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "CEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API ACEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACEnemyAIController();

	FORCEINLINE bool GetisRight() { return isRight; }
	FORCEINLINE void SetisRight(bool InisRight) { isRight = InisRight; }

	FORCEINLINE UBehaviorTree* GetBTree() { return BTree; }
protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
		class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
		class UBehaviorTree* BTree;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
		class UBlackboardComponent* BBComp;
	UPROPERTY(VisibleAnywhere, Category = "Position")
		bool isRight;//오른쪽 생성인지 왼쪽인지
private:
	UFUNCTION(BlueprintCallable, Category = "Perception")
		void OnTargetDetected(AActor* InActor, FAIStimulus Stimulus);
public:
	void SetBlackboardComponent(UBlackboardComponent* NewBlackboard);
};
