// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// Àû
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy", meta = (AllowPrivateAccess = true))
		class ACEnemy* Enemy;
	UPROPERTY(VisibleAnywhere, Category = "EnemyHealth", meta = (BindWidget))
		class UProgressBar* HealthBar;
	
	//TWeakObjectPtr<class ACEnemy> Enemyptr;

public:
	void BindHp(class ACEnemy* BindEnemy);
	void SetHealth();
};
