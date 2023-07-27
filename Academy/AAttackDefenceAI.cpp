// Fill out your copyright notice in the Description page of Project Settings.
#include "CEnemy.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "AAttackDefenceAI.h"

AAAttackDefenceAI::AAAttackDefenceAI()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"))); //퍼셉션 생성
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));//블랙보드 생성

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTAsset(TEXT("BehaviorTree'/Game/Player/Tower/AI/BT_Tower.BT_Tower'"));
	if (BTAsset.Succeeded())
	{
		BTree = BTAsset.Object;
	}
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 800.f; //볼 수 있는 범위
	SightConfig->LoseSightRadius = 800.f;//이미 본 목표물을 유지하는 범위
	SightConfig->PeripheralVisionAngleDegrees = 180.f; //시야 각도
	SightConfig->SetMaxAge(3.0f); //지속시간
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;//모두 인식으로 작성
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());//감지 우선 순위를 시야로 설정
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAAttackDefenceAI::OnTargetDetected);
	//시야에 들어오면 OnTargetDetected함수 호출

	GetPerceptionComponent()->ConfigureSense(*SightConfig);//시야정보 등록
}

void AAAttackDefenceAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (BTree == nullptr) return;
	RunBehaviorTree(BTree); //비헤이비어트리 시작

	if (Blackboard == nullptr) return;
	Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);
	//비트리에 있는 블랙보드를 블랙보드 컴포넌트에 넣어줌
}

void AAAttackDefenceAI::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	if (InActor->ActorHasTag(FName(TEXT("Enemy"))))
	{
		GetBlackboardComponent()->SetValueAsBool(FName(TEXT("isFindEnemy")), true); // Stimulus.WasSuccessfullySensed()
		//블랙보드의 isFindPlayer 값 변경
		TSubclassOf<UAISense> AISense;
		TArray<AActor*> Actors;
		bool isFoundEnemy = false;
		GetPerceptionComponent()->GetCurrentlyPerceivedActors(AISense, Actors); //감지된 액터를 모두 배열에 넣어줌

		for (auto Other : Actors)
		{
			if (ACEnemy* Enemy = Cast<ACEnemy>(Other))
			{
				GetBlackboardComponent()->SetValueAsObject(FName(TEXT("EnemyActor")), Enemy);
				isFoundEnemy = true;
				//블랙보드의 target에 플레이어를 넣어줌
			}
		}
		if (!isFoundEnemy)
		{
			GetBlackboardComponent()->SetValueAsBool(FName(TEXT("isFindEnemy")), false);
		}
	}
}

void AAAttackDefenceAI::SetBlackboardComponent(UBlackboardComponent* NewBlackboard)
{

}
