// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SG_Task_PlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_Task_PlayMontage : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	USG_Task_PlayMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<class UAnimMontage> MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Montage")
	FName NotifyEndName;

	UPROPERTY(EditAnywhere, Category = "Montage")
	bool bWaitForNotify = false;

private:
	FTimerHandle FinishHandle;
	UBehaviorTreeComponent* CurrentOwnerComp;

	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	// 몽타주 종료 시 호출 될 함수
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

};
