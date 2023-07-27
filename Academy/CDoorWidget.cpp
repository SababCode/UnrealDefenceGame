// Fill out your copyright notice in the Description page of Project Settings.
#include "CPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

#include "CDoorWidget.h"

void UCDoorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MyPlayer = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UCDoorWidget::SetCost(int32 InCost)
{
	CostText->SetText(FText::AsNumber(InCost));
}
