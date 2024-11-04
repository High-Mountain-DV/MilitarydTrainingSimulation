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
    bool bDebugBoxOn = true;

    UPROPERTY(EditAnywhere)
    bool bFaceToDirection = true;

    class ASG_Enemy* AIPawn;
private:
    void DebugPoints(const TArray<FVector>& Array);
    bool FindPathPoints();

    TArray<FVector> PathPoints;

    FVector TargetLocation;
    FVector DirectionVector;
    FVector GetDirectionToTarget();
    FVector NextTargetLocation;

    int32 ZeroVelocityCount = 0;
    int32 PointIndex;

    float TempAcceptableRadius = 5;
    float SpeedScale = 0.5f;

    bool ArriveAtLocation(FVector EndLocation, float& OutDist);
    bool bCloseToTargetLocation = false;
    bool bCannotMove;
};
