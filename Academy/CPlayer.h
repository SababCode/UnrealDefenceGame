// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPlayer.generated.h"

DECLARE_DELEGATE(FHealthEvent);
DECLARE_MULTICAST_DELEGATE(FMoneyEvent);

UCLASS()
class ACADEMY_API ACPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void Attack();				//�����Լ�
	void NextAttack();			//�������� �Լ�
	void EndAttack();			//���� ����
	void Dash();				//�޸���
	void EndDash();				//�޸�������
	void Shot();				//�Ѿ� �߻�
	void PlayerRespawn();		//�÷��̾� ������
	void PlayerBuildingBarricade(int32 Num);//�ٸ����̵� ��ġ����
	void PlayerBuildBarricade();	//�ٸ����̵� ��ġ
	void PlayerBuildCancle();		//��ġ ���
	void BuildTick();				//��ġ���� ��ġ�� �ǽð� �̵�
	void UpgradeBuilding();			//���׷��̵�
	//private���� Get, Set�Լ�
	// ���� �� ���Ӱ��� ����
	FORCEINLINE bool GetIsAttack() { return isAttack; }
	FORCEINLINE void SetIsAttack(bool InIsAttack) { isAttack = InIsAttack; }
	FORCEINLINE bool GetIsNextAttack() { return isNextAttack; }
	FORCEINLINE void SetIsNextAttack(bool InisNextAttack) { isNextAttack = InisNextAttack; }
	FORCEINLINE bool GetIsEnableCombo() { return isEnableCombo; }
	FORCEINLINE void SetIsEnableCombo(bool InisEnableCombo) { isEnableCombo = InisEnableCombo; }
	// ������
	FORCEINLINE bool GetIsMove() { return isMove; }
	// ��ȣ�ۿ�
	FORCEINLINE bool GetisStart() { return isStart; }
	FORCEINLINE void SetisStart(bool InisStart) { isStart = InisStart; }
	//�Ǽ�
	FORCEINLINE bool GetisBuild() { return isBuild; }
	FORCEINLINE void SetisBuild(bool InisBuild) { isBuild = InisBuild; }
	// ��
	FORCEINLINE void SetMoney(int32 InMoney) { Money = InMoney; }
	FORCEINLINE int32 GetMoney() { return Money; }
	// ����
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	FORCEINLINE float GetMaxHp() { return MaxHp; }
	//����
	FORCEINLINE bool GetisUpgrade() { return isUpgrade; }
	FORCEINLINE void SetisUpgrade(bool InisUpgrade) { isUpgrade = InisUpgrade; }

	FORCEINLINE AActor* GetUpgradeActor() { return UpgradeActor; }
	FORCEINLINE void SetUpgradeActor(AActor* InUpgradeActor) { UpgradeActor = InUpgradeActor; }

	
	FHealthEvent HealthEvent;
	FMoneyEvent	MoneyEvent;
public :
	UPROPERTY(VisibleAnywhere, Category = "Particle")
		UParticleSystem* FireParticle;


	bool isCanBuild = false;
protected:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	bool isAttack = false;		//���� ��
	bool isNextAttack = false;	//���� ���� ���ɿ���
	bool isEnableCombo = false;	//�޺����� Ȱ��ȭ
	bool isMove = true;			//������ ���ɿ���
	int32 ComboCount = 0;		//�޺�����

	bool isStart = false;		//���� �ߴ���

	float MaxHp = 100.f;		//�ִ� ü��
	float CurrentHp = 100.f;	//���� ü��
	int32 Money = 300;			//��
	int DeadCount = 1;			//������ �ð� �ø���
	bool isBuild = false;		//�Ǽ����� ����
	int BuildNum = -1;			//�Ǽ� ��ȣ

	bool isUpgrade = false;	//���׷��̵� ����

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class UCameraComponent* Camera;			//ī�޶�
	UPROPERTY(VisibleAnywhere, Category = "SpringArm")
		class USpringArmComponent* SpringArm;	//��������
	UPROPERTY(VisibleAnywhere, Category = "Attack")
		class UAnimMontage* AttackMontage;
	UPROPERTY(VisibleAnywhere, Category = "Attack")
		class UAnimMontage* FinalAttackMontage;
	//���� �迭
	UPROPERTY(VisibleAnywhere, Category = "ObjectPool")
		TArray<class ABulletActor*> Bullet;
	//�Ǽ� �迭
	UPROPERTY(VisibleAnywhere, Category = "BuildActor")
		TArray<class AAWoodBarricade*> BuildMesh;	//BuildMesh �迭
	UPROPERTY(VisibleAnywhere, Category = "BuildActor")
		TArray<class ACDefenceTower*> BuildAttackMesh;	//BuildMesh �迭
	UPROPERTY(VisibleAnywhere, Category = "BuildActor")
		AActor* UpgradeActor;//���׷��̵� �� ����
	// * ȿ����
		//�߻���
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* FirstFire;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* SecondFire;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* LastFire;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class UAudioComponent* FireAudioComp;
		//��ġ��
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* BarricadeBuildSound;

	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* ArcherBuildSound;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class UAudioComponent* BulidSoundComp;


	FTimerHandle RespawnTimer;
	FTimerHandle BuildTimerHandle;
};