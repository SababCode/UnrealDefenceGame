// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CEnemy.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

DECLARE_DELEGATE_ThreeParams(FStageEnemy, int32, int32, bool);

UCLASS()
class ACADEMY_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* MyMesh;
	UPROPERTY(VisibleAnywhere, Category = "Particle")
		UParticleSystem* PotalParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyArray", meta = (AllowPrivateAccess = true))
		TArray<ACEnemy*> EnemyList; // 생성된 Enemy를 배열에 넣음
	
	UPROPERTY(VisibleAnywhere, Category = "ZenCount")
		int32 EnemyCount;			// 적 생성 수
	UPROPERTY(VisibleAnywhere, Category = "ZenCount")
		int32 StageCount = 1;		// 스테이지
	UPROPERTY(VisibleAnywhere, Category = "ZenCount")
		int32 TotlaEnemy;

	UPROPERTY(VisibleAnywhere, Category = "PlayerController")
		class ACPlayerController* PlayerController;	//플레이어 컨트롤러
	UPROPERTY(VisibleAnywhere, Category = "Player")
		class ACPlayer* MyPlayer;

	UPROPERTY(VisibleAnywhere, Category = "EffectSound")
		class UAudioComponent* AudioComp;
	UPROPERTY(VisibleAnywhere, Category = "EffectSound")
		TArray<USoundBase*> EnemyZenSound;
	UPROPERTY(VisibleAnywhere, Category = "EffectSound")
		class USoundBase* StartSound;
	FTimerHandle TimerHandle;
protected:
	UFUNCTION()
		void EnemySpawn();
public:
	UFUNCTION()
		void StartSpawner();
	UFUNCTION()
		void BindDead(ACEnemy* Enemy);
	FStageEnemy StageEnemy;
};
