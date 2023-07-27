// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BehaviorTree/BlackboardComponent.h"
#include "CEnemy.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CDefenceTower.generated.h"



UCLASS()
class ACADEMY_API ACDefenceTower : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACDefenceTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* Causer) override;

public:
	void SetMesh();
	void SetCollision();
	void DestroyDefenceActor();
	void Shot();
	void BuildDefenceTower();
	void SetMeshColor();
	void UpgradeBuild(ACDefenceTower* Tower);
	void KillEnemy(ACEnemy* Enemy);
public:

	FORCEINLINE	float GetAttackSpeed() { return AttackSpeed; }
public:
	bool isBuilding = true;			//�Ǽ� ��������
	bool isHightBuild = true;		//�Ǽ��� ������ ��ġ����
	bool isCollisionBuild = true;	//�ݸ����� �Ȱ��ƴ���

protected:
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		TSubclassOf<class UCDoorWidget> DoorWidgetClass;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UCDoorWidget* DoorWidget;

private:
	// ����
	UPROPERTY(VisibleAnywhere, Category = "Cylinder")
		class UStaticMeshComponent* MyCylinder;	//��ħ��
	UPROPERTY(VisibleAnywhere, Category = "Collision")
		class UCapsuleComponent* Collision;		//�浹 �ݸ���
	UPROPERTY(VisibleAnywhere, Category = "LifeText")
		class UTextRenderComponent* LifeText;	//�ؽ�Ʈ
	UPROPERTY(VisibleAnywhere, Category = "Player")
		class ACPlayer* MyPlayer;
	//��ó
	UPROPERTY(VisibleAnywhere, Category = "Archer")
		class USkeletalMesh* ArcherMesh;
	UPROPERTY(VisibleAnywhere, Category = "ArcherAnim")
		class UClass* ArcherAnimInstance;
	// ������ ������
	UPROPERTY(VisibleAnywhere, Category = "EnemyActor")
		TArray<UBlackboardComponent*> EnemyBlackboards; //���� ������ ������ ����
	UPROPERTY(VisibleAnywhere, Category = "MatColor")
		class UMaterialInterface* Mat_Mesh;

	FTimerHandle SpawnSetColiisionTimerHandle;
	FTimerHandle DestroyTimerHandle;
	FCollisionQueryParams Params;


private:
	//����
	float currentHp = 100.f;
	float maxHp = 100.f;
	float AttackSpeed = 1.5f;
	float damage = 20.f;
	bool isAttack = false;
	int32 UpgradeCount = 1;
	int32 UpgradeCost = 60;
};
