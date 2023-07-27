// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "CPlayerController.h"

#include "GameEndWidget.h"

void UGameEndWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
void UGameEndWidget::PausePlayer()
{
	APlayerController* PlayerController = Cast<ACPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->SetPause(true);
		PlayerController->bShowMouseCursor = true;
	}
}
