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

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAnimMontage> MontageToPlay;
	UPROPERTY()
	class USG_EnemyAnimInstance* Anim;

private:
	//// 몽타주 종료 시 호출 될 함수
	//UFUNCTION()
	//void MontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//// 약한 포인터로 컴포넌트 참조 저장
	//TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	//// 바인딩 델리게이트 저장용
	//FOnMontageEnded MontageEndedDelegate;

};
