// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameWidget.h"
#include "CPlayer.h"
#include "CPlayerController.h"
#include "AcademyGameModeBase.h"

AAcademyGameModeBase::AAcademyGameModeBase()
{
	DefaultPawnClass = ACPlayer::StaticClass();
	PlayerControllerClass = ACPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<UGameWidget>
		WidgetAsset(L"WidgetBlueprint'/Game/Player/Blueprint/WB_Game.WB_Game_C'");
	if (WidgetAsset.Succeeded())
	{
		GameWidget = WidgetAsset.Class;
	}
}

void AAcademyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	if (GameWidget != nullptr)
	{
		CreateWidget<UGameWidget>(GetWorld(), GameWidget)->AddToViewport();//À§Á¬ »ý¼º
	}
}