// Fill out your copyright notice in the Description page of Project Settings.

#include "CEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "EnemyHealthWidget.h"


void UEnemyHealthWidget::BindHp(ACEnemy* BindEnemy)
{
	if (BindEnemy == nullptr) return;
	Enemy = BindEnemy;
	Enemy->EnemyHealthEvent.BindUObject(this, &UEnemyHealthWidget::SetHealth);
}

void UEnemyHealthWidget::SetHealth()
{
	HealthBar->SetPercent(Enemy->GetCurrentHp() / Enemy->GetMaxHp());
}
