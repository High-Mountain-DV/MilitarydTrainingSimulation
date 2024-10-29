// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SG_Task_SetAimOffsetAlpha.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_Task_SetAimOffsetAlpha : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	float AimOffsetAlpha;
};
