// Fill out your copyright notice in the Description page of Project Settings.
#include "Components/AudioComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextRenderComponent.h" //확인용

#include "CEnemyAIController.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CDoorWidget.h"

#include "AWoodBarricade.h"

// Sets default values
AAWoodBarricade::AAWoodBarricade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BarricadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barricade"));

	// * 메쉬
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		FullAsset(L"StaticMesh'/Game/InfinityBladeIceLands/Environments/Misc/Exo_Deco01/StaticMesh/SM_Exo_Crate_Open_Intact.SM_Exo_Crate_Open_Intact'");
	if (FullAsset.Succeeded())
	{
		FullBarricade = FullAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		HalfAsset(L"StaticMesh'/Game/InfinityBladeIceLands/Environments/Misc/Exo_Deco01/StaticMesh/SM_Exo_Crate_Open_Damaged.SM_Exo_Crate_Open_Damaged'");
	if (HalfAsset.Succeeded())
	{
		HalfBarricade = HalfAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		BrokenAsset(L"StaticMesh'/Game/InfinityBladeIceLands/Environments/Misc/Exo_Deco01/StaticMesh/SM_Exo_Crate_Open_Destroyed.SM_Exo_Crate_Open_Destroyed'");
	if (BrokenAsset.Succeeded())
	{
		BrokenBarricade = BrokenAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		BuildAsset(L"StaticMesh'/Game/InfinityBladeIceLands/Environments/Misc/Exo_Deco01/StaticMesh/SM_Exo_Crate_Build_Intact.SM_Exo_Crate_Build_Intact'");
	if (BuildAsset.Succeeded())
	{
		BuildBarricade = BuildAsset.Object;
		BarricadeMesh->SetStaticMesh(BuildBarricade);
	}

	// * 콜리전
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(BarricadeMesh);
	BoxCollision->SetRelativeLocation(FVector(0,0,50));
	BoxCollision->SetBoxExtent(FVector(55, 55, 55));
	BoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BoxCollision->SetGenerateOverlapEvents(true);

	// * 대미지를 위한 콜리전
	HitBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBoxCollision->SetupAttachment(BarricadeMesh);
	HitBoxCollision->SetRelativeLocation(FVector(0, 0, 50));
	HitBoxCollision->SetBoxExtent(FVector(45, 45, 45));
	HitBoxCollision->SetCollisionProfileName(TEXT("NoCollision"));


	HitBoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

	 //머티리얼
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		Mat_Asset(L"MaterialInstanceConstant'/Game/InfinityBladeIceLands/Environments/Misc/Exo_Deco01/Materials/M_Build_Inst.M_Build_Inst'");
	if (Mat_Asset.Succeeded())
	{
		Mat_Mesh = Mat_Asset.Object;
		BarricadeMesh->SetMaterial(0, Mat_Mesh);
		BarricadeMesh->SetVectorParameterValueOnMaterials(FName("Color"), FVector(0, 0, 1));
	}

	// * 위젯
	static ConstructorHelpers::FClassFinder<UCDoorWidget>
		OrbWidgetAsset(L"WidgetBlueprint'/Game/Player/Blueprint/WB_Door.WB_Door_C'");
	if (OrbWidgetAsset.Succeeded())
	{
		DoorWidgetClass = OrbWidgetAsset.Class;
	}
	Tags.Add(FName(TEXT("DefenceActor")));

	//확인용 체력 텍스트
	LifeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LifeText"));
	LifeText->SetupAttachment(BarricadeMesh);
	LifeText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	LifeText->TextRenderColor = FColor::White;
	LifeText->SetRelativeLocation(FVector(0, 0, 100));
	LifeText->SetRelativeRotation(FRotator(0, 90, 0));
	LifeText->SetWorldSize(80);
	LifeText->SetText(FText::FromString(FString::FromInt(CurrentHp)));

	//효과음
	BarricadeBrokenSound = CreateDefaultSubobject<UAudioComponent>(TEXT("BrokenSound"));
	BarricadeBrokenSound->SetupAttachment(RootComponent);
	BarricadeBrokenSound->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase>
		BrokenSoundAsset(L"SoundWave'/Game/Music/Effect/BarricadeBroken.BarricadeBroken'");
	if (BrokenSoundAsset.Succeeded())
	{
		BarricadeBrokenSound->SetSound(BrokenSoundAsset.Object);
	}
}

// Called when the game starts or when spawned
void AAWoodBarricade::BeginPlay()
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
	MyPlayer = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	GetWorldTimerManager().SetTimer(SpawnSetCollisionTimerHandle, this, &AAWoodBarricade::SetCollision, 0.1f, false);
}

// Called every frame
void AAWoodBarricade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//텍스트 플레이어 바라보기
	if (MyPlayer != nullptr)
	{
		FVector PlayerLocation = MyPlayer->GetActorLocation();
		FVector LifeTextLocation = LifeText->GetComponentLocation();
		FVector Direction = PlayerLocation - LifeTextLocation;
		Direction.Normalize();

		FRotator Rotation = Direction.Rotation();
		Rotation.Pitch = 0.f;
		LifeText->SetWorldRotation(Rotation);
	}
}

//데미지 받는 함수
float AAWoodBarricade::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* Causer)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, Causer);

	CurrentHp -= Damage;
	SetMesh();

	return Damage;
}

//설치
void AAWoodBarricade::BuildingBarricade()	
{	
	isBuilding = false;							//건설 종료
	isHightBuild = true;					//건설이 가능한 위치인지
	isCollisionBuild = true;				//콜리전이 안겹쳤는지
	BarricadeMesh->SetMaterial(0, nullptr);		//머티리얼 디폴트값
	BarricadeMesh->SetStaticMesh(FullBarricade);//메쉬 넣기
	HitBoxCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	HitBoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
}

void AAWoodBarricade::SetColor()
{
	if (isHightBuild && isCollisionBuild)
	{
		BarricadeMesh->SetVectorParameterValueOnMaterials(FName("Color"), FVector(0, 0, 1));
	}
	else
	{
		BarricadeMesh->SetVectorParameterValueOnMaterials(FName("Color"), FVector(1, 0, 0));
	}
}
//업그레이드 함수
void AAWoodBarricade::UpgradeBuild(AAWoodBarricade* Barricade)
{
	if (MyPlayer->GetMoney() >= Barricade->UpgradeCost)
	{
		Barricade->MaxHp = 100.f * (Barricade->UpgradeCount * 3);
		Barricade->CurrentHp = Barricade->MaxHp;

		Barricade->MyPlayer->SetMoney(MyPlayer->GetMoney() - Barricade->UpgradeCost);

		Barricade->UpgradeCount++;
		Barricade->UpgradeCost = Barricade->UpgradeCount * 40;
		Barricade->SetMesh();

		Barricade->MyPlayer->MoneyEvent.Broadcast();
		Barricade->DoorWidget->SetCost(UpgradeCost);
		SetMesh();
	}
	else
	{
		return;
	}
}

void AAWoodBarricade::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				InEnemy->EnemyKill.AddUObject(this, &AAWoodBarricade::KillEnemy);
				EnemyBlackboards.Add(BlackboardComp);
			}
		}
	}
	if ((OtherActor->ActorHasTag(FName(TEXT("DefenceActor"))) && isBuilding))
	{
		isCollisionBuild = false;
		SetColor();
	}
}

void AAWoodBarricade::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
	if ((OtherActor->ActorHasTag(FName(TEXT("DefenceActor"))) && isBuilding))
	{
		isCollisionBuild = true;
		SetColor();		
	}
}

void AAWoodBarricade::SetMesh()
{
	if (CurrentHp >= 0)
	{
		LifeText->SetText(FText::FromString(FString::FromInt(CurrentHp)));
	}

	if (CurrentHp <= 0) //체력이 0일때
	{
		BarricadeMesh->SetStaticMesh(BrokenBarricade);
		for (int i = 0; i < EnemyBlackboards.Num(); i++)
		{
			EnemyBlackboards[i]->SetValueAsBool(FName(TEXT("isFindDefenceActor")), false);
			EnemyBlackboards[i]->SetValueAsObject(FName(TEXT("DefenceActor")), nullptr);
		}
		BoxCollision->SetCollisionProfileName(TEXT("NoCollision"));
		HitBoxCollision->SetCollisionProfileName(TEXT("NoCollision"));
		BarricadeBrokenSound->Play();
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AAWoodBarricade::DestroyDefenceActor, 1.5f, false);
	}
	else if (CurrentHp <= MaxHp / 2) //절반이하 체력일때
	{
		BarricadeMesh->SetStaticMesh(HalfBarricade);
	}
	else if (CurrentHp > MaxHp / 2)//절반 이상일때
	{
		BarricadeMesh->SetStaticMesh(FullBarricade);
	}
}

void AAWoodBarricade::DestroyDefenceActor()
{
	Destroy();
}

void AAWoodBarricade::SetCollision()
{
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAWoodBarricade::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AAWoodBarricade::OnOverlapEnd);
}

void AAWoodBarricade::KillEnemy(ACEnemy* Enemy)
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

