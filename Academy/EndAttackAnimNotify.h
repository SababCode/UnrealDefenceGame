// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EndAttackAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class ACADEMY_API UEndAttackAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
protected:
	FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
