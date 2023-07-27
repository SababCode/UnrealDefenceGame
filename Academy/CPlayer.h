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
	void Attack();				//공격함수
	void NextAttack();			//다음공격 함수
	void EndAttack();			//공격 종료
	void Dash();				//달리기
	void EndDash();				//달리기종료
	void Shot();				//총알 발사
	void PlayerRespawn();		//플레이어 리스폰
	void PlayerBuildingBarricade(int32 Num);//바리케이드 설치여부
	void PlayerBuildBarricade();	//바리케이드 설치
	void PlayerBuildCancle();		//설치 취소
	void BuildTick();				//설치동안 설치물 실시간 이동
	void UpgradeBuilding();			//업그레이드
	//private변수 Get, Set함수
	// 공격 및 연속공격 여부
	FORCEINLINE bool GetIsAttack() { return isAttack; }
	FORCEINLINE void SetIsAttack(bool InIsAttack) { isAttack = InIsAttack; }
	FORCEINLINE bool GetIsNextAttack() { return isNextAttack; }
	FORCEINLINE void SetIsNextAttack(bool InisNextAttack) { isNextAttack = InisNextAttack; }
	FORCEINLINE bool GetIsEnableCombo() { return isEnableCombo; }
	FORCEINLINE void SetIsEnableCombo(bool InisEnableCombo) { isEnableCombo = InisEnableCombo; }
	// 움직임
	FORCEINLINE bool GetIsMove() { return isMove; }
	// 상호작용
	FORCEINLINE bool GetisStart() { return isStart; }
	FORCEINLINE void SetisStart(bool InisStart) { isStart = InisStart; }
	//건설
	FORCEINLINE bool GetisBuild() { return isBuild; }
	FORCEINLINE void SetisBuild(bool InisBuild) { isBuild = InisBuild; }
	// 돈
	FORCEINLINE void SetMoney(int32 InMoney) { Money = InMoney; }
	FORCEINLINE int32 GetMoney() { return Money; }
	// 스탯
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	FORCEINLINE float GetMaxHp() { return MaxHp; }
	//업글
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
	bool isAttack = false;		//공격 중
	bool isNextAttack = false;	//다음 공격 가능여부
	bool isEnableCombo = false;	//콤보공격 활성화
	bool isMove = true;			//움직임 가능여부
	int32 ComboCount = 0;		//콤보스택

	bool isStart = false;		//시작 했는지

	float MaxHp = 100.f;		//최대 체력
	float CurrentHp = 100.f;	//현재 체력
	int32 Money = 300;			//돈
	int DeadCount = 1;			//리스폰 시간 늘리기
	bool isBuild = false;		//건설상태 인지
	int BuildNum = -1;			//건설 번호

	bool isUpgrade = false;	//업그레이드 여부

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class UCameraComponent* Camera;			//카메라
	UPROPERTY(VisibleAnywhere, Category = "SpringArm")
		class USpringArmComponent* SpringArm;	//스프링암
	UPROPERTY(VisibleAnywhere, Category = "Attack")
		class UAnimMontage* AttackMontage;
	UPROPERTY(VisibleAnywhere, Category = "Attack")
		class UAnimMontage* FinalAttackMontage;
	//슈팅 배열
	UPROPERTY(VisibleAnywhere, Category = "ObjectPool")
		TArray<class ABulletActor*> Bullet;
	//건설 배열
	UPROPERTY(VisibleAnywhere, Category = "BuildActor")
		TArray<class AAWoodBarricade*> BuildMesh;	//BuildMesh 배열
	UPROPERTY(VisibleAnywhere, Category = "BuildActor")
		TArray<class ACDefenceTower*> BuildAttackMesh;	//BuildMesh 배열
	UPROPERTY(VisibleAnywhere, Category = "BuildActor")
		AActor* UpgradeActor;//업그레이드 할 액터
	// * 효과음
		//발사음
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* FirstFire;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* SecondFire;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* LastFire;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class UAudioComponent* FireAudioComp;
		//설치음
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* BarricadeBuildSound;

	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class USoundBase* ArcherBuildSound;
	UPROPERTY(VisibleAnywhere, Category = "EfectSound")
		class UAudioComponent* BulidSoundComp;


	FTimerHandle RespawnTimer;
	FTimerHandle BuildTimerHandle;
};