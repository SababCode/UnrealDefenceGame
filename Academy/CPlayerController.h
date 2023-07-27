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
		void MoveForward(float Axis);	//�� �� �ڷ� �̵�
	UFUNCTION()
		void MoveRight(float Axis);		//������ �� �������� �̵�
	UFUNCTION()
		void Turn(float Axis);			//�¿� ī�޶� ȸ��
	UFUNCTION()
		void LookUp(float Axis);		//���� ī�޶� ȸ��
	UFUNCTION()
		void PlayerJump();				//����
	UFUNCTION()
		void PlayerAttack();			//����
	UFUNCTION()
		void StartStage();				//���� ����
	UFUNCTION()
		void PlayerDash();				//�޸���
	UFUNCTION()
		void PlayerEndDash();			//�޸��� ����
	UFUNCTION()
		void BuildBarricade();			//�ٸ�����Ʈ ��ġ
	UFUNCTION()
		void BuildAttackTower();		//Ÿ�� �Ǽ�
	UFUNCTION()
		void UpgradeBuild();			//Ÿ�� ���׷��̵�

private:
	UPROPERTY(VisibleAnywhere, Category = "Player")
		class ACPlayer* MyPlayer;
	int32 BuildCost = 0;
public:
	FORCEINLINE ACPlayer* GetMyPlayer() { return MyPlayer; }
public:
	FStartStage Stage;
};
