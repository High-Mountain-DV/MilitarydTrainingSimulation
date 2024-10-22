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
    bool PathFindDebug = true;
    class ASG_Enemy* AIPawn;
private:
    FVector TargetLocation;
    bool FindPathPoints();
    int32 PointIndex;
    TArray<FVector> PathPoints;
    FVector NextTargetLocation;
    float SpeedScale;
    bool StartMovement;
    void DebugPoints(const TArray<FVector>& Array);
    FVector DirectionVector;
    FVector GetDirectionToTarget();
    bool ArriveAtLocation(FVector EndLocation, float& OutDist);
    EBTNodeResult::Type StopMovement();
    int32 ZeroVelocityCount = 0;
    bool bCloseToTargetLocation = false;
};
