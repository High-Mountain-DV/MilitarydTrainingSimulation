// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SG_Task_AutoMoveTo.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementType : uint8
{
	Walk_Slow UMETA(DisplayName = "Slow Walk"),
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run")
};
/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_Task_AutoMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	USG_Task_AutoMoveTo();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	EEnemyMovementType MovementType;

	UPROPERTY(EditAnywhere)
	bool bDebugBoxOn = false;

	UPROPERTY(EditAnywhere)
	bool bFaceToDirection = true;

	UPROPERTY(EditAnywhere)
	float AcceptableRadius = 50;
private:
	void DebugPoints(const TArray<FVector>& Array);
	bool FindPathPoints(class ASG_Enemy* Me);
	void InitEnemyVariables(class ASG_Enemy* Me);
};
