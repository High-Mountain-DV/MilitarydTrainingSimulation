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

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MaxAcceleration = 800;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MaxSpeed = 600.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float CurrentSpeed = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float AccelerationRate = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float DecelerationRate = 4.0f;

private:
    FNavPathSharedPtr CurrentPath;
    int32 CurrentPathPoint;
};
