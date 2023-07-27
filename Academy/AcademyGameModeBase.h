// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AcademyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API AAcademyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAcademyGameModeBase();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<class UGameWidget> GameWidget;
};
