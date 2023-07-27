// Fill out your copyright notice in the Description page of Project Settings.

#include "CPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "CEnemyAIController.h"
ACEnemyAIController::ACEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"))); //퍼셉션 생성
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));//블랙보드 생성

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTAsset(TEXT("BehaviorTree'/Game/Enemy/AI/BT_EnemyAI.BT_EnemyAI'"));
	if (BTAsset.Succeeded())
	{
		BTree = BTAsset.Object;
	}
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 300.f; //볼 수 있는 범위
	SightConfig->LoseSightRadius = 400.f;//이미 본 목표물을 유지하는 범위
	SightConfig->PeripheralVisionAngleDegrees = 90.f; //시야 각도
	SightConfig->SetMaxAge(3.0f); //지속시간
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;//모두 인식으로 작성
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());//감지 우선 순위를 시야로 설정
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ACEnemyAIController::OnTargetDetected);
	//시야에 들어오면 OnTargetDetected함수 호출

	GetPerceptionComponent()->ConfigureSense(*SightConfig);//시야정보 등록
}

void ACEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (BTree == nullptr) return;
	RunBehaviorTree(BTree); //비헤이비어트리 시작

	if (Blackboard == nullptr) return;
	Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);	
	//비트리에 있는 블랙보드를 블랙보드 컴포넌트에 넣어줌
}

void ACEnemyAIController::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	if (InActor->ActorHasTag(FName(TEXT("Player"))))
	{
		GetBlackboardComponent()->SetValueAsBool(FName(TEXT("isFindPlayer")), Stimulus.WasSuccessfullySensed());
		//블랙보드의 isFindPlayer 값 변경
		TSubclassOf<UAISense> AISense;
		TArray<AActor*> Actors;
		GetPerceptionComponent()->GetCurrentlyPerceivedActors(AISense, Actors); //감지된 액터를 모두 배열에 넣어줌
		for (auto Other : Actors)
		{
			if (ACPlayer* MyPlayer = Cast<ACPlayer>(Other))
			{
				GetBlackboardComponent()->SetValueAsObject(FName(TEXT("PlayerActor")), MyPlayer);
				//블랙보드의 target에 플레이어를 넣어줌
			}
		}
	}
}

void ACEnemyAIController::SetBlackboardComponent(UBlackboardComponent* NewBlackboard)
{
	Blackboard = NewBlackboard;
	UseBlackboard(Blackboard->GetBlackboardAsset(), Blackboard);
}
