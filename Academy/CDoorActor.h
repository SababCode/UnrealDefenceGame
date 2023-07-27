// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CDoorActor.generated.h"

UCLASS()
class ACADEMY_API ACDoorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACDoorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	////겹침으로 충돌 시작시 호출되는 함수
	//virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	////겹침으로 충돌 종료시 호출되는 함수
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void BrokenDoor();
private:
	UPROPERTY(VisibleAnywhere, Category = "Orb")
		UStaticMeshComponent* Orb;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* BoxCollision;
	UPROPERTY(VisibleAnywhere, Category = "Life")
		int32 LifeCount;
	UPROPERTY(VisibleAnywhere, Category = "LifeText")
		class UTextRenderComponent* LifeText;
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GameEndWidget")
		TSubclassOf<class UGameEndWidget> EndWidgetClass;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GameEndWidget")
		class UGameEndWidget* EndWidget;

	FTimerHandle ColorTimerHandle;
};
