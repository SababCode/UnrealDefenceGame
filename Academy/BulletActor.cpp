// Fill out your copyright notice in the Description page of Project Settings.
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CEnemy.h"

#include "BulletActor.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshAsset(L"StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'");
	if (MeshAsset.Succeeded())
	{
		MyMesh->SetStaticMesh(MeshAsset.Object);
	}
	MyMesh->SetCollisionProfileName("OverlapAllDynamic");
	MyMesh->SetRelativeScale3D(FVector(0.3f));
	RootComponent = MyMesh;
	
	// �ݸ���
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletActor::OnOverlapBegin);
	CollisionComp->SetupAttachment(MyMesh);

	// ������Ÿ��
	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	Projectile->InitialSpeed = 5000.f;//�ʱ� �ӷ�
	Projectile->MaxSpeed = 5000.f; // �ִ�ӷ�
	Projectile->ProjectileGravityScale = 0.0f;//�߷�
	Projectile->bRotationFollowsVelocity = true;//�ӵ��� ���� ȸ��
	Projectile->UpdatedComponent = MyMesh;//�����Ʈ ������Ʈ�� �̵���Ű�� ������Ʈ�ؾ� �ϴ� ������Ʈ
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletActor::SetActive(bool IsActive)
{
	SetActorTickEnabled(IsActive);
	SetActorEnableCollision(IsActive);
	SetActorHiddenInGame(!IsActive);
	bActive = IsActive;

	if (bActive)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABulletActor::Deactiveate, 0.5f, false);
	}
}

void ABulletActor::Deactiveate()
{
	SetActive(false);
}

bool ABulletActor::IsActive()
{
	return bActive;
}

void ABulletActor::FireInDirection(const FVector& Direction)
{
	Projectile->Velocity = Direction * Projectile->InitialSpeed;
}

void ABulletActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFormSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName(TEXT("Enemy"))))
	{	
		ACEnemy* Enemy = Cast<ACEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->TakeDamage(damage, FDamageEvent(), nullptr, this);//�����
			SetActive(false);//��Ȱ��ȭ
		}
	}
}

