// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "UMainWidget.h"


void UUMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (StartBtn)
	{
		StartBtn->OnClicked.AddDynamic(this, &UUMainWidget::StartButtonCall);
	}

	if (ExitBtn)
	{
		ExitBtn->OnClicked.AddDynamic(this, &UUMainWidget::ExitButtonCall);
	}
}

void UUMainWidget::StartButtonCall()
{
	isPlay = true;
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("FrozenCove"));
}

void UUMainWidget::ExitButtonCall()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}
