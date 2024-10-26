// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SG_EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ASG_EnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	FRotator SmoothTargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 30.0f;

public:
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBehaviorTree* BT_Enemy;
};
