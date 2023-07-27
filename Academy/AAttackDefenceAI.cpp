// Fill out your copyright notice in the Description page of Project Settings.
#include "CEnemy.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "AAttackDefenceAI.h"

AAAttackDefenceAI::AAAttackDefenceAI()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"))); //�ۼ��� ����
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));//������ ����

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTAsset(TEXT("BehaviorTree'/Game/Player/Tower/AI/BT_Tower.BT_Tower'"));
	if (BTAsset.Succeeded())
	{
		BTree = BTAsset.Object;
	}
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 800.f; //�� �� �ִ� ����
	SightConfig->LoseSightRadius = 800.f;//�̹� �� ��ǥ���� �����ϴ� ����
	SightConfig->PeripheralVisionAngleDegrees = 180.f; //�þ� ����
	SightConfig->SetMaxAge(3.0f); //���ӽð�
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;//��� �ν����� �ۼ�
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());//���� �켱 ������ �þ߷� ����
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAAttackDefenceAI::OnTargetDetected);
	//�þ߿� ������ OnTargetDetected�Լ� ȣ��

	GetPerceptionComponent()->ConfigureSense(*SightConfig);//�þ����� ���
}

void AAAttackDefenceAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (BTree == nullptr) return;
	RunBehaviorTree(BTree); //�����̺��Ʈ�� ����

	if (Blackboard == nullptr) return;
	Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);
	//��Ʈ���� �ִ� �����带 ������ ������Ʈ�� �־���
}

void AAAttackDefenceAI::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	if (InActor->ActorHasTag(FName(TEXT("Enemy"))))
	{
		GetBlackboardComponent()->SetValueAsBool(FName(TEXT("isFindEnemy")), true); // Stimulus.WasSuccessfullySensed()
		//�������� isFindPlayer �� ����
		TSubclassOf<UAISense> AISense;
		TArray<AActor*> Actors;
		bool isFoundEnemy = false;
		GetPerceptionComponent()->GetCurrentlyPerceivedActors(AISense, Actors); //������ ���͸� ��� �迭�� �־���

		for (auto Other : Actors)
		{
			if (ACEnemy* Enemy = Cast<ACEnemy>(Other))
			{
				GetBlackboardComponent()->SetValueAsObject(FName(TEXT("EnemyActor")), Enemy);
				isFoundEnemy = true;
				//�������� target�� �÷��̾ �־���
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
