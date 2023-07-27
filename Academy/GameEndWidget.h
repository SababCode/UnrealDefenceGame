// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameEndWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UGameEndWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	UPROPERTY(VisibleAnywhere, Category = "UI", meta = (BindWidget))
		class UImage* BlackImage;
	UPROPERTY(VisibleAnywhere, Category = "UI", meta = (BindWidget))
		class UTextBlock* DieText;
	UPROPERTY(VisibleAnywhere, Category = "UI", meta = (BindWidget))
		class UButton* ReTryButton;
	UPROPERTY(VisibleAnywhere, Category = "UI", meta = (BindWidget))
		class UButton* MainButton;
public:
	void PausePlayer();
};
