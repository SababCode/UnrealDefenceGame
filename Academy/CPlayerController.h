// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FStartStage);
UCLASS()
class ACADEMY_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ACPlayerController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
private:
	UFUNCTION()
		void MoveForward(float Axis);	//앞 및 뒤로 이동
	UFUNCTION()
		void MoveRight(float Axis);		//오른쪽 및 왼쪽으로 이동
	UFUNCTION()
		void Turn(float Axis);			//좌우 카메라 회전
	UFUNCTION()
		void LookUp(float Axis);		//상하 카메라 회전
	UFUNCTION()
		void PlayerJump();				//점프
	UFUNCTION()
		void PlayerAttack();			//공격
	UFUNCTION()
		void StartStage();				//게임 시작
	UFUNCTION()
		void PlayerDash();				//달리기
	UFUNCTION()
		void PlayerEndDash();			//달리기 종료
	UFUNCTION()
		void BuildBarricade();			//바리게이트 설치
	UFUNCTION()
		void BuildAttackTower();		//타워 건설
	UFUNCTION()
		void UpgradeBuild();			//타워 업그레이드

private:
	UPROPERTY(VisibleAnywhere, Category = "Player")
		class ACPlayer* MyPlayer;
	int32 BuildCost = 0;
public:
	FORCEINLINE ACPlayer* GetMyPlayer() { return MyPlayer; }
public:
	FStartStage Stage;
};
