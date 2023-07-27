// Fill out your copyright notice in the Description page of Project Settings.

#include "CPlayer.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

#include "GameWidget.h"

void UGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Player = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	SetHealth();
	SetMoney();
	Player->HealthEvent.BindUObject(this, &UGameWidget::SetHealth);
	Player->MoneyEvent.AddUObject(this, &UGameWidget::SetMoney);
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		//스포너 초기화
		EnemySpawner = Cast<AEnemySpawner>(FoundActors[0]);
		EnemySpawner->StageEnemy.BindUObject(this, &UGameWidget::SetBindStage);
	}
	StageText->SetText(FText::FromString(TEXT("Next Stage")));
	StageNumText->SetText(FText::AsNumber(1));
	EnemyCountText->SetText(FText::AsNumber(3));

	//건설 텍스트 초기화
	BuildNum->SetText(FText::FromString(TEXT("1")));
	BuildCost->SetText(FText::AsNumber(30));
	BowBuildNum->SetText(FText::FromString(TEXT("2")));
	BowBuildCost->SetText(FText::AsNumber(50));

	// 건설 색상
	OnTintColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	OffTintColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.f);
	BarricadeImage->SetBrushTintColor(OnTintColor);
	BowImage->SetBrushTintColor(OnTintColor);
}

void UGameWidget::SetHealth()
{
	HealthBar->SetPercent(Player->GetCurrentHp() / Player->GetMaxHp());
	FNumberFormattingOptions Opts;//소수점 같은 자릿수의 포멧을 지정하는 구조체

	Opts.SetMaximumFractionalDigits(0); //소수점을 표시하지 않음
	
	CurrentHealthText->SetText(FText::AsNumber(Player->GetCurrentHp(), &Opts));
	MaxHealthText->SetText(FText::AsNumber(Player->GetMaxHp(), &Opts));
}

void UGameWidget::SetMoney()
{
	CurrentMoney->SetText(FText::AsNumber(Player->GetMoney()));
	BindImageColor();
}

void UGameWidget::SetBindStage(int32 InStage, int32 EnemyCount, bool isStart)
{
	if (!isStart)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SetStage Success"));
	
		StageText->SetText(FText::FromString(TEXT("Next Stage")));
		StageNumText->SetText(FText::AsNumber(InStage));
		EnemyCountText->SetText(FText::AsNumber(EnemyCount));
	}	
	else
	{
		StageText->SetText(FText::FromString(TEXT("Stage")));
		StageNumText->SetText(FText::AsNumber(InStage));
		EnemyCountText->SetText(FText::AsNumber(EnemyCount));
	}	
}

void UGameWidget::BindImageColor()
{
	if (Player->GetMoney() >= 50)
	{
		BowImage->SetBrushTintColor(OnTintColor);
		BarricadeImage->SetBrushTintColor(OnTintColor);
	}
	else if (Player->GetMoney() >= 30)
	{
		BowImage->SetBrushTintColor(OffTintColor);
		BarricadeImage->SetBrushTintColor(OnTintColor);
	}
	else
	{
		BowImage->SetBrushTintColor(OffTintColor);
		BarricadeImage->SetBrushTintColor(OffTintColor);
	}
}
