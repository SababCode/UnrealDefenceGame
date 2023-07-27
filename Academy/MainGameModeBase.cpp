// Fill out your copyright notice in the Description page of Project Settings.
#include "CinematicCamera/Public/CineCameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "UMainWidget.h"
#include "MainGameModeBase.h"

AMainGameModeBase::AMainGameModeBase()
{
	static ConstructorHelpers::FClassFinder<UUMainWidget>
		WidgetAsset(TEXT("WidgetBlueprint'/Game/Main/WBP_Main.WBP_Main_C'"));
	if (WidgetAsset.Succeeded())
	{
		MainWidget = WidgetAsset.Class;
	}
}

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACineCameraActor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		ACineCameraActor* CineCameraActor = Cast<ACineCameraActor>(FoundActors[0]);
		if (CineCameraActor)
		{
			// 시네마 카메라 액터를 메인 카메라로 설정
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			// DefaultPawn 제거
			if (APawn* DefaultPawn = PlayerController->GetPawn())
			{
				DefaultPawn->Destroy();
			}
			PlayerController->SetViewTargetWithBlend(CineCameraActor, 0.f);
			PlayerController->SetIgnoreMoveInput(true);
		}

	}
}
