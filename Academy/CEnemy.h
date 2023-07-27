// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CEnemy.generated.h"

DECLARE_DELEGATE(FEnemyHealthEvent); //체력바 위젯 델리게이트
DECLARE_DELEGATE_OneParam(FEnemyDead, ACEnemy*);//죽었을때 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FEnemyKill, ACEnemy*);

UCLASS()
class ACADEMY_API ACEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FEnemyHealthEvent EnemyHealthEvent;

	FORCEINLINE float GetCurrentHp() { return currentHp; }
	
	FORCEINLINE void SetMaxHp(float InmaxHp) { maxHp = InmaxHp; }
	FORCEINLINE float GetMaxHp() { return maxHp; }

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* Causer) override;
	void SendDamage();
	void ExecuteDead();
private:
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		float damage = 10.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		float currentHp = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		float maxHp = 200.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		float attackRange = 200.f;
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		float attackRadius = 20.f;

	UPROPERTY(VisibleAnywhere, Category = "HealthWidget")
		class UWidgetComponent* HealthWidget;//체력바 위젯

	UPROPERTY(VisibleAnywhere, Category = "AI")
		TArray<AActor*> IgnoreActors;//무시할 액터
public:
	FEnemyDead EnemyDead;
	FEnemyKill EnemyKill;
};
