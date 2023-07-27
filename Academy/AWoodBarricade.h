// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BehaviorTree/BlackboardComponent.h"
#include "CPlayer.h"
#include "CEnemy.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWoodBarricade.generated.h"

UCLASS()
class ACADEMY_API AAWoodBarricade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAWoodBarricade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* Causer) override;
public:
	UFUNCTION()
		void BuildingBarricade();
	UFUNCTION()
		void SetColor();
	UFUNCTION()
		void UpgradeBuild(AAWoodBarricade* Barricade);

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
	UPROPERTY(VisibleAnywhere, Category = "MatColor")
		class UMaterialInterface* Mat_Mesh;

private:
	UPROPERTY(EditAnywhere, Category = "Barricade")
		class UStaticMeshComponent* BarricadeMesh;	//�޽�
	UPROPERTY(EditAnywhere, Category = "Barricade")
		class UStaticMesh* FullBarricade;	//ü���� �ִ��϶�
	UPROPERTY(EditAnywhere, Category = "Barricade")
		class UStaticMesh* HalfBarricade;	//ü���� �����϶�
	UPROPERTY(EditAnywhere, Category = "Barricade")
		class UStaticMesh* BrokenBarricade;//ü���� ������
	UPROPERTY(EditAnywhere, Category = "Barricade")
		class UStaticMesh* BuildBarricade;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* BoxCollision;	//��ħ�� ���� �ݸ���
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* HitBoxCollision;//������� ���� �ݸ���
	
	UPROPERTY(EditAnywhere, Category = "BarricadeHp")
		float CurrentHp = 100.f;
	UPROPERTY(EditAnywhere, Category = "BarricadeHp")
		float MaxHp = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "EnemyActor")
		TArray<UBlackboardComponent*> EnemyBlackboards; //���� ������ ������ ����

	UPROPERTY(VisibleAnywhere, Category = "LifeText")
		class UTextRenderComponent* LifeText;//Ȯ�ο�
	UPROPERTY(VisibleAnywhere, Category = "Player")
		ACPlayer* MyPlayer;
	//ȿ����
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class UAudioComponent* BarricadeBrokenSound;

	int32 UpgradeCount = 1;
	int32 UpgradeCost = 40;
	FTimerHandle SpawnSetCollisionTimerHandle;
	FTimerHandle DestroyTimerHandle;
private:
	void SetMesh();
	void DestroyDefenceActor();
	void SetCollision();
	void KillEnemy(ACEnemy* Enemy);
};
