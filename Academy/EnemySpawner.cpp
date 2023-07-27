// Fill out your copyright notice in the Description page of Project Settings.
#include "Components/AudioComponent.h"

#include "CPlayerController.h"
#include "CPlayer.h"
#include "CEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_GetEndLocation.h"

#include "particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "EnemySpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sapwner"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		ParticleAsset(L"ParticleSystem'/Game/Model/P_Potal_Effect.P_Potal_Effect'");
	if (ParticleAsset.Succeeded())
	{
		PotalParticle = ParticleAsset.Object;
	}

	// 효과음
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComp->SetupAttachment(RootComponent);
	AudioComp->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase>
		StartSoundAsset(L"SoundWave'/Game/Music/StageStart.StageStart'");
	if (StartSoundAsset.Succeeded())
	{
		StartSound = StartSoundAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>
		EnemyZenSoundAsset1(L"SoundWave'/Game/Music/Enemy/Zombie1.Zombie1'");
	if (EnemyZenSoundAsset1.Succeeded())
	{
		EnemyZenSound.Add(EnemyZenSoundAsset1.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>
		EnemyZenSoundAsset2(L"SoundWave'/Game/Music/Enemy/Zombie2.Zombie2'");
	if (EnemyZenSoundAsset2.Succeeded())
	{
		EnemyZenSound.Add(EnemyZenSoundAsset2.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>
		EnemyZenSoundAsset3(L"SoundWave'/Game/Music/Enemy/Zombie3.Zombie3'");
	if (EnemyZenSoundAsset3.Succeeded())
	{
		EnemyZenSound.Add(EnemyZenSoundAsset3.Object);
	}
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ACPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PlayerController == nullptr) return;
	PlayerController->Stage.BindUObject(this, &AEnemySpawner::StartSpawner);

	MyPlayer = Cast<ACPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (MyPlayer == nullptr) return;

	UGameplayStatics::SpawnEmitterAttached(PotalParticle, MyMesh);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::EnemySpawn()
{
	if (EnemyCount > 0)
	{
		int32 RandomInt = FMath::RandRange(0, EnemyZenSound.Num() - 1);
		AudioComp->SetSound(EnemyZenSound[RandomInt]);
		AudioComp->Play();
		FRotator rotator(0, 0, 0);
		FVector Vec = GetActorLocation() + FVector(-50,0,0);
		ACEnemy* EnemyZen = GetWorld()->SpawnActor<ACEnemy>(Vec, rotator);
		EnemyZen->EnemyDead.BindUObject(this, &AEnemySpawner::BindDead);
		EnemyList.Add(EnemyZen);
		EnemyCount--;
	}
	else if(EnemyCount <= 0)
	{
		EnemyCount = 0;
		//루프 타이머 취소
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}

void AEnemySpawner::StartSpawner()
{
	AudioComp->SetSound(StartSound);
	AudioComp->Play();
	EnemyCount = StageCount * 3;
	TotlaEnemy = EnemyCount;
	StageEnemy.Execute(StageCount, TotlaEnemy, true);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemySpawner::EnemySpawn, 3.f, true);
}

void AEnemySpawner::BindDead(ACEnemy* Enemy)
{
	EnemyList.Remove(Enemy);
	TotlaEnemy--;
	if (EnemyList.Num() == 0)
	{
		PlayerController->GetMyPlayer()->SetisStart(false);
		StageCount++;
		StageEnemy.Execute(StageCount, StageCount * 3, false);
	}
	else if (EnemyList.Num() > 0)
	{		
		StageEnemy.Execute(StageCount, TotlaEnemy, true);

		//스테이지 끝나면 보상 금액
		if (MyPlayer->MoneyEvent.IsBound())
		{
			MyPlayer->SetMoney(MyPlayer->GetMoney() + (30 * StageCount));
			MyPlayer->MoneyEvent.Broadcast();
		}
	}
}
