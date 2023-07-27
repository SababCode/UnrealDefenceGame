// Fill out your copyright notice in the Description page of Project Settings.
#include "GameEndWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"

#include "CPlayer.h"
#include "CEnemy.h"

#include "CDoorActor.h"

// Sets default values
ACDoorActor::ACDoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//  * 메쉬

	Orb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Orb"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> 
		OrbAsset (L"StaticMesh'/Game/InfinityBladeIceLands/Environments/Ice/Ice_Fortress/StaticMesh/SM_Ice_Fort_Pillar_1b.SM_Ice_Fort_Pillar_1b'");
	if (OrbAsset.Succeeded())
	{
		Orb->SetStaticMesh(OrbAsset.Object);
	}
	RootComponent = Orb;
	Orb->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	//OverlapAllDynamic: 오브젝트 타입은 Dynamic, 모든타입과 겹침
	Orb->SetGenerateOverlapEvents(true);

	Orb->SetRelativeRotation(FRotator(0, 0,0));
	Orb->SetRelativeScale3D(FVector(2.f));

	// * 위젯
	static ConstructorHelpers::FClassFinder<UGameEndWidget>
		EndWidgetAsset(L"WidgetBlueprint'/Game/Player/Blueprint/WB_GameEnd.WB_GameEnd_C'");
	if (EndWidgetAsset.Succeeded())
	{
		EndWidgetClass = EndWidgetAsset.Class;
	}

	// * 충돌 이벤트를 위한 박스 콜리전
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(Orb);
	BoxCollision->SetRelativeLocation(FVector(0, 0, 50));
	BoxCollision->SetBoxExtent(FVector(100, 100, 100));
	BoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BoxCollision->SetGenerateOverlapEvents(true);

	// 라이프 및 태그
	LifeCount = 5;
	
	Tags.Add(FName(TEXT("Orb")));	

	//텍스트
	LifeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LifeText"));
	LifeText->SetupAttachment(Orb);
	LifeText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	LifeText->TextRenderColor = FColor::White;
	LifeText->SetRelativeLocation(FVector(0, 0, 150));
	LifeText->SetRelativeRotation(FRotator(0, 90, 0));
	LifeText->SetWorldSize(120);
	LifeText->SetText(FText::FromString(FString::FromInt(LifeCount)));
}

// Called when the game starts or when spawned
void ACDoorActor::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACDoorActor::OnOverlapBegin);
	if (EndWidgetClass)
	{
		EndWidget = Cast<UGameEndWidget>(CreateWidget<UGameEndWidget>(GetWorld(), EndWidgetClass));
		if (EndWidget)
		{
			EndWidget->AddToViewport();
			EndWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Called every frame
void ACDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

//충돌 이벤트 Begin
void ACDoorActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName(TEXT("Enemy"))))
	{
		ACEnemy* Enemy = Cast<ACEnemy>(OtherActor);
		Enemy->ExecuteDead();
		OtherActor->Destroy();
		LifeCount--;
		LifeText->SetText(FText::FromString(FString::FromInt(LifeCount)));
		if (LifeCount == 0)
		{
			BrokenDoor();
		}
	}
}

void ACDoorActor::BrokenDoor()
{
	EndWidget->PausePlayer();
	EndWidget->SetVisibility(ESlateVisibility::Visible);
}
