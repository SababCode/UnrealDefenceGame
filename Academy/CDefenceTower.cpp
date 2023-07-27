// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletActor.h"
#include "DrawDebugHelpers.h"
#include "CDoorWidget.h"

#include "CEnemyAIController.h"

#include "CPlayer.h"
#include "AAttackDefenceAI.h"
#include "ATowerAnimInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

#include "CDefenceTower.h"

// Sets default values
ACDefenceTower::ACDefenceTower()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메쉬
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		ArcherAsset(L"SkeletalMesh'/Game/Tower/Modle/Archer.Archer'");
	if (ArcherAsset.Succeeded())
	{
		ArcherMesh = ArcherAsset.Object;
	}

	// 메쉬 위치 선정
	RootComponent = GetCapsuleComponent();
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	// 받침대
	MyCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		CylinderAsset(L"StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'");
	if (CylinderAsset.Succeeded())
	{
		MyCylinder->SetStaticMesh(CylinderAsset.Object);
	}
	MyCylinder->SetupAttachment(GetMesh());
	MyCylinder->SetRelativeLocation(FVector(0, 0, -100));
	MyCylinder->SetCollisionProfileName(TEXT("NoCollision"));
	// 애니메이션
	static ConstructorHelpers::FClassFinder<UATowerAnimInstance>
		ArcherAnimAsset(L"AnimBlueprint'/Game/Tower/Animation/ABP_Archer.ABP_Archer_C'");
	if (ArcherAnimAsset.Succeeded())
	{
		ArcherAnimInstance = ArcherAnimAsset.Class;
	}

	// AI
	AIControllerClass = AAAttackDefenceAI::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// * 콜리전
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoxCollision"));
	Collision->SetupAttachment(GetMesh());
	Collision->SetCapsuleHalfHeight(150.f);
	Collision->SetCapsuleRadius(90.f);
	Collision->SetRelativeLocation(FVector(0, 0, 130.f));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	GetCapsuleComponent()->SetCapsuleRadius(50.f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 체력 텍스트
	LifeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LifeText"));
	LifeText->SetupAttachment(GetMesh());
	LifeText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	LifeText->TextRenderColor = FColor::White;
	LifeText->SetRelativeLocation(FVector(0, 0, 200));
	LifeText->SetRelativeRotation(FRotator(0, 90, 0));
	LifeText->SetWorldSize(80);
	LifeText->SetText(FText::FromString(FString::FromInt(currentHp)));

	//머티리얼
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		Mat_Asset(L"MaterialInstanceConstant'/Game/InfinityBladeIceLands/Environments/Misc/Exo_Deco01/Materials/M_Build_Inst.M_Build_Inst'");
	if (Mat_Asset.Succeeded())
	{
		Mat_Mesh = Mat_Asset.Object;
		MyCylinder->SetMaterial(0, Mat_Mesh);
		MyCylinder->SetVectorParameterValueOnMaterials(FName("Color"), FVector(0, 0, 1));
	}
	// * 위젯
	static ConstructorHelpers::FClassFinder<UCDoorWidget>
		OrbWidgetAsset(L"WidgetBlueprint'/Game/Player/Blueprint/WB_Door.WB_Door_C'");
	if (OrbWidgetAsset.Succeeded())
	{
		DoorWidgetClass = OrbWidgetAsset.Class;
	}

	// 태그
	Tags.Add(FName(TEXT("DefenceActor")));
}

// Called when the game starts or when spawned
void ACDefenceTower::BeginPlay()
{
	Super::BeginPlay();
	
	if (DoorWidgetClass != nullptr)
	{
		DoorWidget = Cast<UCDoorWidget>(CreateWidget<UUserWidget>(GetWorld(), DoorWidgetClass));
		if (DoorWidget != nullptr)
		{
			DoorWidget->AddToViewport();
			DoorWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	//플레이어
	MyPlayer = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//메쉬
	GetMesh()->SetSkeletalMesh(ArcherMesh);
	//애니메이션
	GetMesh()->SetAnimClass(ArcherAnimInstance);

	//0.1초 후에 Overlap연결하기
	GetWorldTimerManager().SetTimer(SpawnSetColiisionTimerHandle, this, &ACDefenceTower::SetCollision, 0.1f, false);
}

// Called every frame
void ACDefenceTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//텍스트가 플레이어를 바라보게
	ACPlayer* Player = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player != nullptr)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector LifeTextLocation = LifeText->GetComponentLocation();
		FVector Direction = PlayerLocation - LifeTextLocation;
		Direction.Normalize();

		FRotator Rotation = Direction.Rotation();
		Rotation.Pitch = 0.f;
		LifeText->SetWorldRotation(Rotation);
	}
}

// Called to bind functionality to input
void ACDefenceTower::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ACDefenceTower::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* Causer)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, Causer);

	currentHp -= Damage;
	SetMesh();

	return Damage;
}

void ACDefenceTower::SetMesh()
{
	if (currentHp >= 0)
	{
		LifeText->SetText(FText::FromString(FString::FromInt(currentHp)));
	}

	if (currentHp <= 0) //체력이 0일때
	{
		for (int i = 0; i < EnemyBlackboards.Num(); i++)
		{
			EnemyBlackboards[i]->SetValueAsBool(FName(TEXT("isFindDefenceActor")), false);
			EnemyBlackboards[i]->SetValueAsObject(FName(TEXT("DefenceActor")), nullptr);
		}
		Collision->SetCollisionProfileName(TEXT("NoCollision"));
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ACDefenceTower::DestroyDefenceActor, 1.5f, false);
	}
}

void ACDefenceTower::SetCollision()
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACDefenceTower::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACDefenceTower::OnOverlapEnd);
}

void ACDefenceTower::DestroyDefenceActor()
{
	Destroy();
}

void ACDefenceTower::Shot()
{
	AAAttackDefenceAI* TowerAI = Cast<AAAttackDefenceAI>(GetController());
	if (TowerAI == nullptr) return;
	AActor* Target = Cast<AActor>(TowerAI->GetBlackboardComponent()->GetValueAsObject(FName(TEXT("EnemyActor"))));
	if (Target == nullptr) return;

	FVector Start, End, Direction;
	Start = GetActorLocation();
	Direction = (Target->GetActorLocation() - Start).GetSafeNormal();
	//정규화
	End = (Direction * 800) + Start;
	//정규화된 방향으로 800 + Start의 값
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FHitResult Result;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.5f);

	if (GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECollisionChannel::ECC_Pawn, Params))
	{
		if (Result.GetActor()->ActorHasTag(FName(TEXT("DefenceActor"))))
		{
			Params.AddIgnoredActor(Result.GetActor());
			Shot();
		}
		if (Result.GetActor()->ActorHasTag(FName(TEXT("Enemy"))))
		{
			UGameplayStatics::ApplyDamage(Result.GetActor(), damage, GetController(), this, UDamageType::StaticClass());
			Params.ClearIgnoredActors();
		}
	}
}

void ACDefenceTower::BuildDefenceTower()
{
	isBuilding = false;							//건설 종료
	isHightBuild = true;					//건설이 가능한 위치인지
	isCollisionBuild = true;				//콜리전이 안겹쳤는지
	MyCylinder->SetMaterial(0, nullptr);		//머티리얼 디폴트값
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACDefenceTower::SetMeshColor()
{
	if (isHightBuild && isCollisionBuild)
	{
		MyCylinder->SetVectorParameterValueOnMaterials(FName("Color"), FVector(0, 0, 1));
	}
	else
	{
		MyCylinder->SetVectorParameterValueOnMaterials(FName("Color"), FVector(1, 0, 0));
	}
}

void ACDefenceTower::UpgradeBuild(ACDefenceTower* Tower)
{
	if (MyPlayer->GetMoney() >= Tower->UpgradeCost)
	{
		Tower->maxHp = 100.f * (Tower->UpgradeCount * 3);
		Tower->currentHp = Tower->maxHp;
		Tower->damage = 20.f + (UpgradeCount * 15.f);
		Tower->MyPlayer->SetMoney(MyPlayer->GetMoney() - Tower->UpgradeCost);

		Tower->UpgradeCount++;
		Tower->UpgradeCost = Tower->UpgradeCount * 60;
		Tower->SetMesh();

		Tower->MyPlayer->MoneyEvent.Broadcast();
		Tower->DoorWidget->SetCost(UpgradeCost);
	}
	else
	{
		return;
	}
}

void ACDefenceTower::KillEnemy(ACEnemy* Enemy)
{
	ACEnemyAIController* AIController = Cast<ACEnemyAIController>(Enemy->GetInstigatorController());
	if (AIController == nullptr) return;
	
	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (BBComp == nullptr) return;
	for (int i = 0; i < EnemyBlackboards.Num(); i++)
	{
		if (BBComp == EnemyBlackboards[i])
		{			
			EnemyBlackboards.Remove(BBComp);			
			return;
		}
	}	
}

void ACDefenceTower::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName(TEXT("Player"))))
	{
		MyPlayer->SetisUpgrade(true);
		MyPlayer->SetUpgradeActor(Cast<AActor>(this));
		if (DoorWidget->GetVisibility() == ESlateVisibility::Hidden)//NextStage 위젯이 안보일때
		{
			DoorWidget->SetCost(UpgradeCost);
			DoorWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (OtherActor->ActorHasTag(FName(TEXT("Enemy"))))
	{
		ACEnemy* InEnemy = Cast<ACEnemy>(OtherActor);
		ACEnemyAIController* AIController = Cast<ACEnemyAIController>(OtherActor->GetInstigatorController());
		if (AIController)
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp == nullptr) return;
			BlackboardComp->SetValueAsBool(FName(TEXT("isFindDefenceActor")), true);
			BlackboardComp->SetValueAsObject(FName(TEXT("DefenceActor")), this);

			if (!EnemyBlackboards.Contains(BlackboardComp))
			{
				InEnemy->EnemyKill.AddUObject(this, &ACDefenceTower::KillEnemy);
				EnemyBlackboards.Add(BlackboardComp);
			}

		}
	}
	if (OtherActor->ActorHasTag(FName(TEXT("DefenceActor"))) && isBuilding)
	{
		isCollisionBuild = false;
		SetMeshColor();
	}
}

void ACDefenceTower::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName(TEXT("Player"))))
	{
		MyPlayer->SetisUpgrade(false);
		MyPlayer->SetUpgradeActor(nullptr);

		if (DoorWidget->GetVisibility() == ESlateVisibility::Visible)//NextStage 위젯이 보일때
		{
			DoorWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (OtherActor->ActorHasTag(FName(TEXT("DefenceActor"))) && isBuilding)
	{
		isCollisionBuild = true;
		SetMeshColor();
	}
}

