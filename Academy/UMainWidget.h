// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UUMainWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, Category = "Button", meta = (BindWidget))
		class UButton* StartBtn;
	UPROPERTY(VisibleAnywhere, Category = "Button", meta = (BindWidget))
		class UButton* ExitBtn;
	UFUNCTION()
		void ExitButtonCall();
	UFUNCTION()
		void StartButtonCall();
public:
	FORCEINLINE bool GetisPlay() { return isPlay; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Play", meta = (AllowprivateAssecc = true))
		bool isPlay;
};
