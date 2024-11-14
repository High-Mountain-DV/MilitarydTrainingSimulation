// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "SG_Task_MoveTo.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_Task_MoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:
    USG_Task_MoveTo();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere)
    bool bRun = false;

    UPROPERTY(EditAnywhere)
    bool bDebugBoxOn = false;

    UPROPERTY(EditAnywhere)
    bool bFaceToDirection = true;

    UPROPERTY(EditAnywhere)
    bool bRemoveTargetLocationKey = true;

private:
    void DebugPoints(const TArray<FVector>& Array);
    bool FindPathPoints(class ASG_Enemy* Me);
    void InitEnemyVariables(class ASG_Enemy* Me);
};
