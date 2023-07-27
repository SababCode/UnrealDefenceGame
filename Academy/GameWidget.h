// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	//UPROPERTY(VisibleAnywhere, Category = "GameUI", meta = (BindWidget))
	//	class UText* TimeText;
	
	//�÷��̾�
	UPROPERTY(BlueprintReadWrite ,EditAnywhere, Category = "Player", meta = (AllowPrivateAccess = true))
		class ACPlayer* Player;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemySpawner", meta = (AllowPrivateAccess = true))
		class AEnemySpawner* EnemySpawner;
	//ü�� ���α׷��� ��
	UPROPERTY(VisibleAnywhere, Category = "HealthUI", meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(VisibleAnywhere, Category = "HealthUI", meta = (BindWidget))
		class UTextBlock* CurrentHealthText;
	UPROPERTY(VisibleAnywhere, Category = "HealthUI", meta = (BindWidget))
		class UTextBlock* MaxHealthText;
	//�� �ؽ�Ʈ
	UPROPERTY(VisibleAnywhere, Category = "HealthUI", meta = (BindWidget))
		class UTextBlock* CurrentMoney;

	//�������� �� �� �� �ؽ�Ʈ
	UPROPERTY(VisibleAnywhere, Category = "StageUI", meta = (BindWidget))
		class UTextBlock* StageText;
	UPROPERTY(VisibleAnywhere, Category = "StageUI", meta = (BindWidget))
		class UTextBlock* EnemyCountText;
	UPROPERTY(VisibleAnywhere, Category = "StageUI", meta = (BindWidget))
		class UTextBlock* StageNumText;

	//��ġ�� �̹��� �� �ؽ�Ʈ 1
	UPROPERTY(VisibleAnywhere, Category = "BuildUI", meta = (BindWidget))
		class UImage* BarricadeImage;
	UPROPERTY(VisibleAnywhere, Category = "BuildUI", meta = (BindWidget))
		class UTextBlock* BuildNum;
	UPROPERTY(VisibleAnywhere, Category = "BuildUI", meta = (BindWidget))
		class UTextBlock* BuildCost;
	//2
	UPROPERTY(VisibleAnywhere, Category = "BuildUI", meta = (BindWidget))
		class UImage* BowImage;
	UPROPERTY(VisibleAnywhere, Category = "BuildUI", meta = (BindWidget))
		class UTextBlock* BowBuildNum;
	UPROPERTY(VisibleAnywhere, Category = "BuildUI", meta = (BindWidget))
		class UTextBlock* BowBuildCost;
private:
	FLinearColor OnTintColor;
	FLinearColor OffTintColor;
public:
	void SetHealth();
	void SetMoney();
	void SetBindStage(int32 InStage, int32 EnemyCount, bool isStart);
	void BindImageColor();
};
