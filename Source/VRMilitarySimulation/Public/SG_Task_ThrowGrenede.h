// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SG_Task_ThrowGrenede.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_Task_ThrowGrenede : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	USG_Task_ThrowGrenede();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	float TimeMultiplier = 0.7f;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* AM_Throw_Grenede;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* AM_Toss_Grenede;
	
	UPROPERTY(EditAnywhere)
    bool bDebugBoxOn = false;

	UPROPERTY()
	class ASG_Enemy* Me;

	UPROPERTY()
	class USG_EnemyAnimInstance* Anim;

private:
	FVector GetThrowVelocityToTarget(const FVector& TargetLocation);
	bool CheckTrajectoryCollision(const FVector& TargetLocation, const FVector& Velocity);

	FVector TargetPoint;
	UBehaviorTreeComponent* CurrentOwnerComp;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void GrabGrenedeAtRightHandNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
		
	UFUNCTION()
	void ThrowGrenedeNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UPROPERTY()
	class ASG_Grenede* Grenede;
};
