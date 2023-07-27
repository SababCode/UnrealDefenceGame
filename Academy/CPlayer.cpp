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
	// * �ʱ� �ӷ�
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	// * �޽�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(L"SkeletalMesh'/Game/ParagonLtBelica/Characters/Heroes/Belica/Meshes/Belica.Belica'");
	/*"SkeletalMesh'/Game/Player/Modle/Player-Pose.Player-Pose'"*/
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));


	// * ������ ��
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent()); //ĸ���� ����
	SpringArm->TargetArmLength = 500.f;
	SpringArm->SetRelativeRotation(FRotator(-35, 0, 0));
	SpringArm->bUsePawnControlRotation = true; //��Ʈ�ѷ� ȸ���� ������ ����
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = false; //�浹 ����

	// * ī�޶�
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));//ī�޶� ����
	Camera->SetupAttachment(SpringArm); //ī�޶� �������Ͽ� ��� ��ŵ�ϴ�.

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// * �ִϸ��̼�
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(L"AnimBlueprint'/Game/Player/Blueprint/ABP_BPlayer.ABP_BPlayer_C'");
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}

	// * ��Ÿ��
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

	// * ��ƼŬ
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		FireParticleAsset(TEXT("ParticleSystem'/Game/ParagonLtBelica/FX/Particles/Belica/Abilities/Primary/FX/P_BelicaMuzzle.P_BelicaMuzzle'"));

	if (FireParticleAsset.Succeeded())
	{
		FireParticle = FireParticleAsset.Object;
	}

	// ȿ����
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
	//�Ǽ� ȿ����
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
	//���� ����ġ�� 100��ŭ ������ ��ǥ
	ABulletActor* BulletObj = GetWorld()->SpawnActor<ABulletActor>(ABulletActor::StaticClass(), Transform);
	//�Ѿ��� ����
	BulletObj->SetActive(false); //�Ѿ��� ��Ȱ��ȭ
	Bullet.Add(BulletObj); //�迭�� �߰�	
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
		if (!isAttack)//�������� �ƴ϶��
		{
			FireAudioComp->SetSound(FirstFire);
			FireAudioComp->Play();
			UGameplayStatics::SpawnEmitterAttached(FireParticle, GetMesh(), FName("SMG_Barrel"));
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
			//isMove = false;
			PlayAnimMontage(AttackMontage, 1.5f, FName(TEXT("Attack1")));
			//��Ÿ�ֿ��� ���� Attack1�� ����
			Shot();
			isAttack = true;
			//���ݻ��·� ����
			ComboCount++;
			//�޺� ī��Ʈ ����
		}
		else //�������ε� �Լ��� ����Ǹ� ���� ������ Ȱ��ȭ
		{
			isNextAttack = true;
		}
	}
}

void ACPlayer::NextAttack()
{
	if (!GetMovementComponent()->IsFalling())
	{
		if (isNextAttack)//���������� Ȱ��ȭ ���¶��
		{
			if (ComboCount == 1)//�޺� ī��Ʈ�� 1�̶��
			{
				FireAudioComp->SetSound(SecondFire);
				FireAudioComp->Play();
				UGameplayStatics::SpawnEmitterAttached(FireParticle, GetMesh(), FName("SMG_Barrel"));
				//isMove = false;
				GetCharacterMovement()->MaxWalkSpeed = 400.f;
				PlayAnimMontage(AttackMontage, 1.5f, FName(TEXT("Attack2")));
				isNextAttack = false;//2��° ������ �Ǿ��ٸ� false
				Shot();
				ComboCount++;//�޺� ī��Ʈ ����
			}
			else if (ComboCount == 2)//�޺�ī��Ʈ�� 2���
			{
				FireAudioComp->SetSound(LastFire);
				FireAudioComp->Play();
				UGameplayStatics::SpawnEmitterAttached(FireParticle, GetMesh(), FName("SMG_Barrel"));
				//isMove = false;
				GetCharacterMovement()->MaxWalkSpeed = 400.f;
				PlayAnimMontage(FinalAttackMontage, 1.3f);
				//������ ���� ����� �� ����
				Shot();
				isNextAttack = false;
				ComboCount = 0;//�޺� ī��Ʈ �ʱ�ȭ
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
			Item->SetDamage((10.f * ComboCount) + 10.0f);//�����Ǿ��ִ� �迭�� ����� ����
			Item->SetActive(true);
			Item->SetActorRelativeLocation(GetActorLocation() + (GetActorForwardVector() * 50.f));
			Item->FireInDirection(GetActorForwardVector());
			return;
		}
	}
	FTransform Transform;
	Transform.SetLocation(GetActorLocation() + (GetActorForwardVector() * 50.f));//���� �տ� ��ġ

	ABulletActor* BulletObj = GetWorld()->SpawnActor<ABulletActor>(ABulletActor::StaticClass(), Transform);
	//�Ѿ��� ���ư��� ������ ���� �չ���

	BulletObj->SetActive(true);	//�Ѿ� Ȱ��ȭ
	BulletObj->FireInDirection(GetActorForwardVector());//�Ѿ� ����(���� �� ��ġ��)

	BulletObj->SetDamage((10.f * ComboCount) + 10.0f);	//���� ������ �Ѿ˿� ����� ����

	Bullet.Add(BulletObj);		//�Ѿ��� �迭�� �߰�
}
//�÷��̾� ������
void ACPlayer::PlayerRespawn()
{
	DeadCount++;
	SetActorLocation(FVector(-299.f, 1355.f, -389.f));
	CurrentHp = MaxHp;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);// ������ Ȱ��ȭ
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // �ݸ��� Ȱ��ȭ
	if (HealthEvent.IsBound())
	{
		HealthEvent.Execute(); //��������Ʈ ���ε�� �Լ� ����
	}
}
//�ٸ�����Ʈ ��ġ �غ�
void ACPlayer::PlayerBuildingBarricade(int32 Num)
{
	AAWoodBarricade* BarricadeActor = nullptr;
	ACDefenceTower* AttackTower = nullptr;
	switch (Num)//��ġ ����
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
//�ٸ�����Ʈ ��ġ
void ACPlayer::PlayerBuildBarricade()
{
	switch (BuildNum)
	{
	case 0:
		if (BuildMesh.Num() > 0 &&
			BuildMesh[BuildMesh.Num() - 1]->isHightBuild &&
			BuildMesh[BuildMesh.Num() - 1]->isCollisionBuild)//�迭�� ������� �ʰ� ��ġ ȯ���� ��������
		{
			BulidSoundComp->SetSound(BarricadeBuildSound);
			BulidSoundComp->Play();
			GetWorldTimerManager().ClearTimer(BuildTimerHandle);//�ð� �ʱ�ȭ
			BuildMesh[BuildMesh.Num() - 1]->BuildingBarricade();//�������� ���� BuildMesh�� ��ġ ���ش�.
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
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->isCollisionBuild)//�迭�� ������� �ʰ� ��ġ ȯ���� ��������
		{
			BulidSoundComp->SetSound(ArcherBuildSound);
			BulidSoundComp->Play();
			GetWorldTimerManager().ClearTimer(BuildTimerHandle);//�ð� �ʱ�ȭ
			BuildAttackMesh[BuildAttackMesh.Num() - 1]->BuildDefenceTower();//�������� ���� BuildMesh�� ��ġ ���ش�.
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
//�ٸ�����Ʈ ��ġ ���
void ACPlayer::PlayerBuildCancle()
{
	GetWorldTimerManager().ClearTimer(BuildTimerHandle); //�ð� �ʱ�ȭ
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
//�ٸ�����Ʈ �ǽð� �˻�
void ACPlayer::BuildTick()
{
	//��ġ �ʱ�ȭ
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
		if (BuildAttackMesh[BuildAttackMesh.Num() - 1]->GetActorLocation().Z >= -400)//���� ���� ��ġ �Ұ�
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
		HealthEvent.Execute(); //��������Ʈ ���ε�� �Լ� ����
	}
	if (CurrentHp <= 0)
	{
		SetActorLocation(FVector(-299.f, 49.f, -78.f));
		GetWorldTimerManager().SetTimer(RespawnTimer, this, &ACPlayer::PlayerRespawn, 5.f * DeadCount, false);
	}
	return Damage;
}

