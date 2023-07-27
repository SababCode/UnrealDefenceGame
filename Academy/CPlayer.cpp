// Fill out your copyright notice in the Description page of Project Settings.
#include "Components/AudioComponent.h"

#include "CDefenceTower.h"
#include "AWoodBarricade.h"

#include "BulletActor.h"

#include "particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "CDoorWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "CPlayer.h"

// Sets default values
ACPlayer::ACPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// * Tag
	Tags.Add(FName(TEXT("Player")));
	// * 초기 속력
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	// * 메쉬
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(L"SkeletalMesh'/Game/ParagonLtBelica/Characters/Heroes/Belica/Meshes/Belica.Belica'");
	/*"SkeletalMesh'/Game/Player/Modle/Player-Pose.Player-Pose'"*/
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));


	// * 스프링 암
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent()); //캡슐에 부착
	SpringArm->TargetArmLength = 500.f;
	SpringArm->SetRelativeRotation(FRotator(-35, 0, 0));
	SpringArm->bUsePawnControlRotation = true; //컨트롤러 회전에 영향을 받음
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = false; //충돌 안함

	// * 카메라
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));//카메라 생성
	Camera->SetupAttachment(SpringArm); //카메라를 스프링암에 상속 시킵니다.

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// * 애니메이션
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(L"AnimBlueprint'/Game/Player/Blueprint/ABP_BPlayer.ABP_BPlayer_C'");
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}

	// * 몽타주
	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		AttackMtg(L"AnimMontage'/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Primary_Fire_Combo_Montage.Primary_Fire_Combo_Montage'");
	if (AttackMtg.Succeeded())
	{
		AttackMontage = AttackMtg.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		FinalAttackMtg(L"AnimMontage'/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Primary_Fire_Final_Montage.Primary_Fire_Final_Montage'");
	if (FinalAttackMtg.Succeeded())
	{
		FinalAttackMontage = FinalAttackMtg.Object;
	}

	// * 파티클
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		FireParticleAsset(TEXT("ParticleSystem'/Game/ParagonLtBelica/FX/Particles/Belica/Abilities/Primary/FX/P_BelicaMuzzle.P_BelicaMuzzle'"));

	if (FireParticleAsset.Succeeded())
	{
		FireParticle = FireParticleAsset.Object;
	}

	// 효과음
	FireAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSound"));
	FireAudioComp->SetupAttachment(RootComponent);
	FireAudioComp->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase>
		FirstSoundAsset(L"SoundWave'/Game/Music/Effect/Rifle_FirstFire.Rifle_FirstFire'");
	if (FirstSoundAsset.Succeeded())
	{
		FirstFire = FirstSoundAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>
		SecondSoundAsset(L"SoundWave'/Game/Music/Effect/RifleSecondFire.RifleSecondFire'");
	if (SecondSoundAsset.Succeeded())
	{
		SecondFire = SecondSoundAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>
		LastSoundAsset(L"SoundWave'/Game/Music/Effect/RifleLastFire.RifleLastFire'");
	if (LastSoundAsset.Succeeded())
	{
		LastFire = LastSoundAsset.Object;
	}
	//건설 효과음
	BulidSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BuildSound"));
	BulidSoundComp->SetupAttachment(RootComponent);
	BulidSoundComp->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase>
		BarricadeSound(L"SoundWave'/Game/Music/Effect/SpawnBarricade.SpawnBarricade'");
	if (BarricadeSound.Succeeded())
	{
		BarricadeBuildSound = BarricadeSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>
		TowerSoundAsset(L"SoundWave'/Game/Music/Effect/Male_Laugh.Male_Laugh'");
	if (TowerSoundAsset.Succeeded())
	{
		ArcherBuildSound = TowerSoundAsset.Object;
	}
}

// Called when the game starts or when spawned
void ACPlayer::BeginPlay()
{
	Super::BeginPlay();
	FTransform Transform;

	Transform.SetLocation(GetActorLocation() + (GetActorForwardVector() * 50.f));
	//폰의 앞위치에 100만큼 떨어진 좌표
	ABulletActor* BulletObj = GetWorld()->SpawnActor<ABulletActor>(ABulletActor::StaticClass(), Transform);
	//총알을 생성
	BulletObj->SetActive(false); //총알을 비활성화
	Bullet.Add(BulletObj); //배열에 추가	
}

// Called every frame
void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACPlayer::Attack()
{
	if (!GetMovementComponent()->IsFalling())
	{
		if (!isAttack)//공격중이 아니라면
		{
			FireAudioComp->SetSound(FirstFire);
			FireAudioComp->Play();
			UGameplayStatics::SpawnEmitterAttached(FireParticle, GetMesh(), FName("SMG_Barrel"));
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
			//isMove = false;
			PlayAnimMontage(AttackMontage, 1.5f, FName(TEXT("Attack1")));
			//몽타주에서 섹션 Attack1을 실행
			Shot();
			isAttack = true;
			//공격상태로 변경
			ComboCount++;
			//콤보 카운트 증가
		}
		else //공격중인데 함수가 실행되면 다음 공격이 활성화
		{
			isNextAttack = true;
		}
	}
}

void ACPlayer::NextAttack()
{
	if (!GetMovementComponent()->IsFalling())
	{
		if (isNextAttack)//다음공격이 활성화 상태라면
		{
			if (ComboCount == 1)//콤보 카운트가 1이라면
			{
				FireAudioComp->SetSound(SecondFire);
				FireAudioComp->Play();
				UGameplayStatics::SpawnEmitterAttached(FireParticle, GetMesh(), FName("SMG_Barrel"));
				//isMove = false;
				GetCharacterMovement()->MaxWalkSpeed = 400.f;
				PlayAnimMontage(AttackMontage, 1.5f, FName(TEXT("Attack2")));
				isNextAttack = false;//2번째 공격이 되었다면 false
				Shot();
				ComboCount++;//콤보 카운트 증가
			}
			else if (ComboCount == 2)//콤보카운트가 2라면
			{
				FireAudioComp->SetSound(LastFire);
				FireAudioComp->Play();
				UGameplayStatics::SpawnEmitterAttached(FireParticle, GetMesh(), FName("SMG_Barrel"));
				//isMove = false;
				GetCharacterMovement()->MaxWalkSpeed = 400.f;
				PlayAnimMontage(FinalAttackMontage, 1.3f);
				//마지막 공격 모션이 들어갈 예정
				Shot();
				isNextAttack = false;
				ComboCount = 0;//콤보 카운트 초기화
			}
		}
	}
}

void ACPlayer::EndAttack()
{
	isMove = true;
	isAttack = false;
	isNextAttack = false;
	ComboCount = 0;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void ACPlayer::Dash()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ACPlayer::EndDash()
{
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void ACPlayer::Shot()
{
	for (ABulletActor* Item : Bullet)
	{
		if (!Item->IsActive())
		{
			Item->SetDamage((10.f * ComboCount) + 10.0f);//생성되어있는 배열에 대미지 설정
			Item->SetActive(true);
			Item->SetActorRelativeLocation(GetActorLocation() + (GetActorForwardVector() * 50.f));
			Item->FireInDirection(GetActorForwardVector());
			return;
		}
	}
	FTransform Transform;
	Transform.SetLocation(GetActorLocation() + (GetActorForwardVector() * 50.f));//폰의 앞에 위치

	ABulletActor* BulletObj = GetWorld()->SpawnActor<ABulletActor>(ABulletActor::StaticClass(), Transform);
	//총알이 날아가는 방향은 폰의 앞방향

	BulletObj->SetActive(true);	//총알 활성화
	BulletObj->FireInDirection(GetActorForwardVector());//총알 생성(폰의 앞 위치에)

	BulletObj->SetDamage((10.f * ComboCount) + 10.0f);	//새로 생성된 총알에 대미지 설정

	Bullet.Add(BulletObj);		//총알을 배열에 추가
}
//플레이어 리스폰
void ACPlayer::PlayerRespawn()
{
	DeadCount++;
	SetActorLocation(FVector(-299.f, 1355.f, -389.f));
	CurrentHp = MaxHp;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);// 움직임 활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 콜리전 활성화
	if (HealthEvent.IsBound())
	{
		HealthEvent.Execute(); //델리게이트 바인드된 함수 실행
	}
}
//바리게이트 설치 준비
void ACPlayer::PlayerBuildingBarricade(int32 Num)
{
	AAWoodBarricade* BarricadeActor = nullptr;
	ACDefenceTower* AttackTower = nullptr;
	switch (Num)//설치 종류
	{
	case 0:
		BarricadeActor = GetWorld()->SpawnActor<AAWoodBarricade>(AAWoodBarricade::StaticClass());
		BuildMesh.Add(BarricadeActor);
		BuildNum = 0;
		break;
	case 1:
		AttackTower = GetWorld()->SpawnActor<ACDefenceTower>(ACDefenceTower::StaticClass());
		BuildAttackMesh.Add(AttackTower);
		BuildNum = 1;
		break;
	}
	GetWorldTimerManager().SetTimer(BuildTimerHandle, this, &ACPlayer::BuildTick, 0.01f, true);	
}
//바리게이트 설치
void ACPlayer::PlayerBuildBarricade()
{
	switch (BuildNum)
	{
	case 0:
		if (BuildMesh.Num() > 0 &&
			BuildMesh[BuildMesh.Num() - 1]->isHightBuild &&
			BuildMesh[BuildMesh.Num() - 1]->isCollisionBuild)//배열이 비어있지 않고 설치 환경이 괜찮은지
		{
			BulidSoundComp->SetSound(BarricadeBuildSound);
			BulidSoundComp->Play();
			GetWorldTimerManager().ClearTimer(BuildTimerHandle);//시간 초기화
			BuildMesh[BuildMesh.Num() - 1]->BuildingBarricade();//마지막에 들어온 BuildMesh를 설치 해준다.
			isBuild = false;
			isCanBuild = true;
			BuildNum = -1;
		}
		else
		{
			isCanBuild = false;
		}
		break;
	case 1:
		if (BuildAttackMesh.Num() > 0 &&
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->isHightBuild &&
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->isCollisionBuild)//배열이 비어있지 않고 설치 환경이 괜찮은지
		{
			BulidSoundComp->SetSound(ArcherBuildSound);
			BulidSoundComp->Play();
			GetWorldTimerManager().ClearTimer(BuildTimerHandle);//시간 초기화
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->BuildDefenceTower();//마지막에 들어온 BuildMesh를 설치 해준다.
			isBuild = false;
			isCanBuild = true;
			BuildNum = -1;
		}
		else
		{
			isCanBuild = false;
		}
		break;
	}
}
//바리게이트 설치 취소
void ACPlayer::PlayerBuildCancle()
{
	GetWorldTimerManager().ClearTimer(BuildTimerHandle); //시간 초기화
	switch (BuildNum)
	{
	case 0:
		if (BuildMesh.Num() > 0)
		{
			BuildMesh[BuildMesh.Num() - 1]->Destroy();
			BuildMesh.RemoveAt(BuildMesh.Num() - 1);
			BuildNum = -1;
		}
		break;
	case 1:
		if (BuildAttackMesh.Num() > 0)
		{
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->Destroy();
			BuildAttackMesh.RemoveAt(BuildAttackMesh.Num() - 1);
			BuildNum = -1;
		}
		break;
	}
}
//바리게이트 실시간 검사
void ACPlayer::BuildTick()
{
	//위치 초기화
	switch (BuildNum)
	{
	case 0:
		BuildMesh[BuildMesh.Num() - 1]->SetActorLocation(GetActorLocation() + (GetActorForwardVector() * 200) + FVector(0, 0, -100.f));
		if (BuildMesh[BuildMesh.Num() - 1]->GetActorLocation().Z >= -490)
		{
			BuildMesh[BuildMesh.Num() - 1]->isHightBuild = false;
		}
		else
		{
			BuildMesh[BuildMesh.Num() - 1]->isHightBuild = true;
		}
		BuildMesh[BuildMesh.Num() - 1]->SetColor();
		break;
	case 1:
		BuildAttackMesh[BuildAttackMesh.Num() - 1]->SetActorLocation(GetActorLocation() + (GetActorForwardVector() * 200));
		if (BuildAttackMesh[BuildAttackMesh.Num() - 1]->GetActorLocation().Z >= -400)//일정 높이 설치 불가
		{
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->isHightBuild = false;
		}
		else
		{
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->isHightBuild = true;
		}
		BuildAttackMesh[BuildAttackMesh.Num() - 1]->SetMeshColor();
		break;
	}
}

void ACPlayer::UpgradeBuilding()
{
	AAWoodBarricade* Bari = Cast<AAWoodBarricade>(UpgradeActor);
	if (Bari != nullptr)
	{
		Bari->UpgradeBuild(Bari);
		return;
	}
	ACDefenceTower* Tower = Cast<ACDefenceTower>(UpgradeActor);
	if (Tower != nullptr)
	{
		Tower->UpgradeBuild(Tower);
		return;
	}
}

float ACPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	CurrentHp -= Damage;
	if (HealthEvent.IsBound())
	{
		HealthEvent.Execute(); //델리게이트 바인드된 함수 실행
	}
	if (CurrentHp <= 0)
	{
		SetActorLocation(FVector(-299.f, 49.f, -78.f));
		GetWorldTimerManager().SetTimer(RespawnTimer, this, &ACPlayer::PlayerRespawn, 5.f * DeadCount, false);
	}
	return Damage;
}

