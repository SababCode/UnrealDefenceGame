// Fill out your copyright notice in the Description page of Project Settings.
#include "CPlayer.h"
#include "CPlayerController.h"


ACPlayerController::ACPlayerController()
{

}

//���� ��Ʈ�ѷ��� ���� ������ �� ȣ��Ǵ� �Լ��̴�.
void ACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!IsValid(InPawn)) return; //���� ������ ���� �ʴٸ�
	MyPlayer = Cast<ACPlayer>(InPawn);//������ �ִ� ���� ACPlayer�� ��ȯ�������
}


void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// * �̵�
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ACPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ACPlayerController::MoveRight);

	// * ȸ��
	InputComponent->BindAxis(TEXT("Turn"), this, &ACPlayerController::Turn);
	InputComponent->BindAxis(TEXT("LookUp"), this, &ACPlayerController::LookUp);

	// * ����
	InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACPlayerController::PlayerJump);

	// * ����
	InputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ACPlayerController::PlayerAttack);

	// * ���� ����
	InputComponent->BindAction(TEXT("StartStage"), IE_Pressed, this, &ACPlayerController::StartStage);
	// * �뽬
	InputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ACPlayerController::PlayerDash);
	InputComponent->BindAction(TEXT("Dash"), IE_Released, this, &ACPlayerController::PlayerEndDash);

	// * �ٸ�����Ʈ
	InputComponent->BindAction(TEXT("Barricade"), IE_Pressed, this, &ACPlayerController::BuildBarricade);

	// * Ÿ��
	InputComponent->BindAction(TEXT("AttackTower"), IE_Pressed, this, &ACPlayerController::BuildAttackTower);
	InputComponent->BindAction(TEXT("Upgrade"), IE_Pressed, this, &ACPlayerController::UpgradeBuild);
}

void ACPlayerController::MoveForward(float Axis)
{
	if (Axis != 0.f && MyPlayer->GetIsMove()) //�������� ���� 0.f�� �ƴҶ�
	{
		const FRotator Rot = GetControlRotation();//��Ʈ�ѷ��� ȸ������ �����´�.
		const FRotator YawRot(0, Rot.Yaw, 0);//ȸ�������� Yaw���� �����մϴ�.
		const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X); 
		//YawRot���� �������� X���� ���͸� ��ȯ�մϴ�.(��, �յڹ����� ã�´�)

		GetPawn()->AddMovementInput(Direction, Axis);
		/* GetPawn�� ����Ͽ� ���� ��Ʈ�ѷ��� �����ϰ��ִ� Pawn����
		AddMovementInput���� �������� �߰��ϴ� �Լ��� ����ϰ�
		Direction: �չ������� / Axis: �̵���ġ ��ŭ �̵������ش�. */
	}
}

void ACPlayerController::MoveRight(float Axis)
{
	if (Axis != 0.f && MyPlayer->GetIsMove())
	{
		const FRotator Rot = GetControlRotation();
		const FRotator YawRot(0, Rot.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		//Yaw������ Y���� ���͸� ��ȯ�մϴ�.(��, �¿����)

		GetPawn()->AddMovementInput(Direction, Axis);
	}
}

void ACPlayerController::Turn(float Axis)
{
	AddYawInput(Axis);
}

void ACPlayerController::LookUp(float Axis)
{
	AddPitchInput(Axis);
}

void ACPlayerController::PlayerJump()
{
	if (IsValid(MyPlayer))
	{
		MyPlayer->Jump();
	}
}

void ACPlayerController::PlayerAttack()
{
	if (IsValid(MyPlayer)&& !MyPlayer->GetisBuild())
	{
		MyPlayer->Attack();
	}
	else if (IsValid(MyPlayer) && MyPlayer->GetisBuild())
	{
		MyPlayer->PlayerBuildBarricade();
		if (!MyPlayer->isCanBuild) return;
		MyPlayer->SetMoney(MyPlayer->GetMoney() - BuildCost);
		MyPlayer->MoneyEvent.Broadcast();
	}
}

void ACPlayerController::StartStage()
{
	if (!MyPlayer->GetisStart())
	{
		//���� ���۽� �ൿ
		if (Stage.IsBound())
		{		
			MyPlayer->SetisStart(true);
			Stage.Execute();			
		}
	}
}


void ACPlayerController::PlayerDash()
{
	if (!MyPlayer->GetIsAttack())
	{
		MyPlayer->Dash();
	}
}

void ACPlayerController::PlayerEndDash()
{
	if (!MyPlayer->GetIsAttack())
	{
		MyPlayer->EndDash();
	}
}

void ACPlayerController::BuildBarricade()
{
	//1�� ��������
	if (MyPlayer->GetMoney() < 30) return;
	BuildCost = 30;
	if (!MyPlayer->GetisBuild()) //�Ǽ����°� �ƴ϶��
	{
		MyPlayer->PlayerBuildingBarricade(0);
		MyPlayer->SetisBuild(true);
	}
	else if (MyPlayer->GetisBuild())//�Ǽ����°� �´ٸ�
	{
		MyPlayer->PlayerBuildCancle();
		MyPlayer->SetisBuild(false);
	}
}

void ACPlayerController::BuildAttackTower()
{
	//2�� ��������
	if (MyPlayer->GetMoney() < 50) return;
	BuildCost = 50;
	if (!MyPlayer->GetisBuild()) //�Ǽ����°� �ƴ϶��
	{
		MyPlayer->PlayerBuildingBarricade(1);
		MyPlayer->SetisBuild(true);
	}
	else if (MyPlayer->GetisBuild())//�Ǽ����°� �´ٸ�
	{
		MyPlayer->PlayerBuildCancle();
		MyPlayer->SetisBuild(false);
	}
}

void ACPlayerController::UpgradeBuild()
{
	if (MyPlayer->GetisUpgrade())
	{
		MyPlayer->UpgradeBuilding();
	}
	else
	{
		return;
	}
}
