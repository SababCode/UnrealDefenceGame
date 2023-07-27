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
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"))); //�ۼ��� ����
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));//������ ����

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTAsset(TEXT("BehaviorTree'/Game/Enemy/AI/BT_EnemyAI.BT_EnemyAI'"));
	if (BTAsset.Succeeded())
	{
		BTree = BTAsset.Object;
	}
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 300.f; //�� �� �ִ� ����
	SightConfig->LoseSightRadius = 400.f;//�̹� �� ��ǥ���� �����ϴ� ����
	SightConfig->PeripheralVisionAngleDegrees = 90.f; //�þ� ����
	SightConfig->SetMaxAge(3.0f); //���ӽð�
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;//��� �ν����� �ۼ�
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());//���� �켱 ������ �þ߷� ����
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ACEnemyAIController::OnTargetDetected);
	//�þ߿� ������ OnTargetDetected�Լ� ȣ��

	GetPerceptionComponent()->ConfigureSense(*SightConfig);//�þ����� ���
}

void ACEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (BTree == nullptr) return;
	RunBehaviorTree(BTree); //�����̺��Ʈ�� ����

	if (Blackboard == nullptr) return;
	Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);	
	//��Ʈ���� �ִ� �����带 ������ ������Ʈ�� �־���
}

void ACEnemyAIController::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	if (InActor->ActorHasTag(FName(TEXT("Player"))))
	{
		GetBlackboardComponent()->SetValueAsBool(FName(TEXT("isFindPlayer")), Stimulus.WasSuccessfullySensed());
		//�������� isFindPlayer �� ����
		TSubclassOf<UAISense> AISense;
		TArray<AActor*> Actors;
		GetPerceptionComponent()->GetCurrentlyPerceivedActors(AISense, Actors); //������ ���͸� ��� �迭�� �־���
		for (auto Other : Actors)
		{
			if (ACPlayer* MyPlayer = Cast<ACPlayer>(Other))
			{
				GetBlackboardComponent()->SetValueAsObject(FName(TEXT("PlayerActor")), MyPlayer);
				//�������� target�� �÷��̾ �־���
			}
		}
	}
}

void ACEnemyAIController::SetBlackboardComponent(UBlackboardComponent* NewBlackboard)
{
	Blackboard = NewBlackboard;
	UseBlackboard(Blackboard->GetBlackboardAsset(), Blackboard);
}
