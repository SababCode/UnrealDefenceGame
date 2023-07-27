// Fill out your copyright notice in the Description page of Project Settings.
#include "CPlayer.h"
#include "CPlayerController.h"


ACPlayerController::ACPlayerController()
{

}

//현재 컨트롤러가 폰을 소유할 때 호출되는 함수이다.
void ACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!IsValid(InPawn)) return; //폰을 가지고 있지 않다면
	MyPlayer = Cast<ACPlayer>(InPawn);//가지고 있는 폰을 ACPlayer로 변환시켜줘라
}


void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// * 이동
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ACPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ACPlayerController::MoveRight);

	// * 회전
	InputComponent->BindAxis(TEXT("Turn"), this, &ACPlayerController::Turn);
	InputComponent->BindAxis(TEXT("LookUp"), this, &ACPlayerController::LookUp);

	// * 점프
	InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACPlayerController::PlayerJump);

	// * 공격
	InputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ACPlayerController::PlayerAttack);

	// * 게임 시작
	InputComponent->BindAction(TEXT("StartStage"), IE_Pressed, this, &ACPlayerController::StartStage);
	// * 대쉬
	InputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ACPlayerController::PlayerDash);
	InputComponent->BindAction(TEXT("Dash"), IE_Released, this, &ACPlayerController::PlayerEndDash);

	// * 바리게이트
	InputComponent->BindAction(TEXT("Barricade"), IE_Pressed, this, &ACPlayerController::BuildBarricade);

	// * 타워
	InputComponent->BindAction(TEXT("AttackTower"), IE_Pressed, this, &ACPlayerController::BuildAttackTower);
	InputComponent->BindAction(TEXT("Upgrade"), IE_Pressed, this, &ACPlayerController::UpgradeBuild);
}

void ACPlayerController::MoveForward(float Axis)
{
	if (Axis != 0.f && MyPlayer->GetIsMove()) //움직임의 값이 0.f이 아닐때
	{
		const FRotator Rot = GetControlRotation();//컨트롤러의 회전값을 가져온다.
		const FRotator YawRot(0, Rot.Yaw, 0);//회전값에서 Yaw값만 추출합니다.
		const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X); 
		//YawRot값을 기준으로 X축의 벡터를 반환합니다.(즉, 앞뒤방향을 찾는다)

		GetPawn()->AddMovementInput(Direction, Axis);
		/* GetPawn을 사용하여 현재 컨트롤러를 소유하고있는 Pawn에서
		AddMovementInput으로 움직임을 추가하는 함수를 사용하고
		Direction: 앞방향으로 / Axis: 이동수치 만큼 이동시켜준다. */
	}
}

void ACPlayerController::MoveRight(float Axis)
{
	if (Axis != 0.f && MyPlayer->GetIsMove())
	{
		const FRotator Rot = GetControlRotation();
		const FRotator YawRot(0, Rot.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		//Yaw값에서 Y축의 벡터를 반환합니다.(즉, 좌우방향)

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
		//게임 시작시 행동
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
	//1을 눌렀을때
	if (MyPlayer->GetMoney() < 30) return;
	BuildCost = 30;
	if (!MyPlayer->GetisBuild()) //건설상태가 아니라면
	{
		MyPlayer->PlayerBuildingBarricade(0);
		MyPlayer->SetisBuild(true);
	}
	else if (MyPlayer->GetisBuild())//건설상태가 맞다면
	{
		MyPlayer->PlayerBuildCancle();
		MyPlayer->SetisBuild(false);
	}
}

void ACPlayerController::BuildAttackTower()
{
	//2를 눌렀을때
	if (MyPlayer->GetMoney() < 50) return;
	BuildCost = 50;
	if (!MyPlayer->GetisBuild()) //건설상태가 아니라면
	{
		MyPlayer->PlayerBuildingBarricade(1);
		MyPlayer->SetisBuild(true);
	}
	else if (MyPlayer->GetisBuild())//건설상태가 맞다면
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
