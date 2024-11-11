// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BTNode.h"
#include "SG_Task_ThrowGrenade.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_Task_ThrowGrenade : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	USG_Task_ThrowGrenade();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	float TimeMultiplier = 0.7f;

	UPROPERTY(EditAnywhere)
    bool bDebugBoxOn = false;
};
