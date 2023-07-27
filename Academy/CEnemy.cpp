// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "CDefenceTower.h"
#include "AAttackDefenceAI.h"

#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Components/ProgressBar.h"

#include "EnemyHealthWidget.h"
#include "CPlayer.h"
#include "CEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "CEnemy.h"

// Sets default values
ACEnemy::ACEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// * �޽�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(L"SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Meshes/Grux.Grux'");
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	this->GetCapsuleComponent()->SetCapsuleHalfHeight(83.f);

	// * �ִϸ��̼�
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(L"AnimBlueprint'/Game/Enemy/Animation/ABP_Enemy.ABP_Enemy_C'");
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimClass(AnimAsset.Class);
	}

	// * AI
	AIControllerClass = ACEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Tags.Add(FName(TEXT("Enemy")));

	// * ü�¹�
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBar"));
	static ConstructorHelpers::FClassFinder<UEnemyHealthWidget>
		HealthWidgetAsset(L"WidgetBlueprint'/Game/Enemy/Blueprint/WBP_EnemyHealth.WBP_EnemyHealth_C'");
	if (HealthWidgetAsset.Succeeded())
	{
		HealthWidget->SetWidgetClass(HealthWidgetAsset.Class);
	}
	HealthWidget->SetupAttachment(GetMesh());
	HealthWidget->SetRelativeLocation(FVector(0, 0, 250.f));
	HealthWidget->SetRelativeRotation(FRotator(0, 90.0f, 0));
	HealthWidget->SetWidgetSpace(EWidgetSpace::World);

	// * �̵��ӵ�
	GetCharacterMovement()->MaxWalkSpeed = 600;

}

// Called when the game starts or when spawned
void ACEnemy::BeginPlay()
{
	Super::BeginPlay();
	currentHp = maxHp;

	UEnemyHealthWidget* EnemyWidget = Cast<UEnemyHealthWidget>(HealthWidget->GetUserWidgetObject());
	if (EnemyWidget != nullptr)
	{
		EnemyWidget->BindHp(this);
	}
}

// Called every frame
void ACEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ACPlayer* Player = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player != nullptr)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector HealthBarLocation = HealthWidget->GetComponentLocation();
		FVector Direction = PlayerLocation - HealthBarLocation;
		Direction.Normalize();

		FRotator Rotation = Direction.Rotation();
		Rotation.Pitch = 0.f;
		HealthWidget->SetWorldRotation(Rotation);
	}
}

// Called to bind functionality to input
void ACEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ACEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* Causer)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, Causer);
	UBlackboardComponent* BB = Cast<UBlackboardComponent>(GetController());
	currentHp -= Damage;

	if (EnemyHealthEvent.IsBound())
	{
		EnemyHealthEvent.Execute();
	}
	if (currentHp <= 0)
	{
		ACPlayer* Player = Cast<ACPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (Player)
		{
			if (Player->MoneyEvent.IsBound())
			{
				Player->SetMoney(Player->GetMoney() + 10);
				Player->MoneyEvent.Broadcast();
			}
		}		
		ExecuteDead();
		Destroy();
	}

	return Damage;
}

void ACEnemy::SendDamage()
{
	FVector Start, End, Direction;
	Start = GetActorLocation();//������ ��ġ
	Direction = GetActorForwardVector();//������ ����
	End = Start + (Direction * attackRange);//�������� ���� ���濡�� ���ݻ�Ÿ������� �Ÿ�
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; //�浹 ������Ʈ �����迭
	TEnumAsByte<EObjectTypeQuery> PawnType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);//��Ÿ��
	ObjectTypes.Add(PawnType);
	IgnoreActors.Add(this);
	FHitResult HitResult;
	//ForDuration
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), Start, End, attackRadius, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 0.5f))
	{
		if (IsValid(HitResult.GetActor()))
		{
			if (HitResult.GetActor()->ActorHasTag(FName(TEXT("Enemy"))))
			{
				IgnoreActors.Add(HitResult.GetActor());
				SendDamage();
			}
			if (HitResult.GetActor()->ActorHasTag(FName(TEXT("Player"))) 
				|| HitResult.GetActor()->ActorHasTag(FName(TEXT("DefenceActor")))) //Player �Ǵ� DefenceActor�� ��������
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), damage, GetController(), this, UDamageType::StaticClass());//������ ����
			}
		}
	}
}

void ACEnemy::ExecuteDead()
{
	if (EnemyDead.IsBound())
	{
		EnemyKill.Broadcast(this);
		EnemyDead.Execute(this);
	}
}

