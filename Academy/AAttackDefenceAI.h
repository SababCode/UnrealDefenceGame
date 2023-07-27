// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AIPerceptionTypes.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "AAttackDefenceAI.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API AAAttackDefenceAI : public AAIController
{
	GENERATED_BODY()
public:
	AAAttackDefenceAI();
protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
		class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
		class UBehaviorTree* BTree;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
		class UBlackboardComponent* BBComp;

private:
	UFUNCTION(BlueprintCallable, Category = "Perception")
		void OnTargetDetected(AActor* InActor, FAIStimulus Stimulus);
public:
	void SetBlackboardComponent(UBlackboardComponent* NewBlackboard);
};
