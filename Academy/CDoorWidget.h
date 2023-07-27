// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDoorWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UCDoorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "DoorUse", meta = (BindWidget))
		class UTextBlock* DescreptionText;
	UPROPERTY(VisibleAnywhere, Category = "DoorUse", meta = (BindWidget))
		class UTextBlock* UseText;
	UPROPERTY(VisibleAnywhere, Category = "Cost", meta = (BindWidget))
		class UTextBlock* CostText;
private:
	UPROPERTY(VisibleAnywhere, Category = "Player")
		class ACPlayer* MyPlayer;
public:
	void SetCost(int32 InCost);
};
