// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletActor.generated.h"

UCLASS()
class ACADEMY_API ABulletActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetActive(bool IsActive);//Ȱ��ȭ ��Ȱ��ȭ �Լ�
	void Deactiveate(); //��Ȱ��ȭ �Լ�
	bool IsActive();//Ȱ��ȭ ��Ȱ��ȭ ����
	void FireInDirection(const FVector& Direction);

	FORCEINLINE void SetDamage(float Indamage) { damage = Indamage; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* MyMesh;
	UPROPERTY(VisibleAnywhere, Category = "Progectile")
		class UProjectileMovementComponent* Projectile;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		class USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category = "Damage")
		float damage;

	bool bActive; //Ȱ��ȭ ��Ȱ��ȭ ����
	FTimerHandle TimerHandle;

private:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFormSweep, const FHitResult& SweepResult);
};
