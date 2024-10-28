// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_PlayMontage.h"
#include "AIController.h"
#include "../SG_Enemy.h"

USG_Task_PlayMontage::USG_Task_PlayMontage()
{
	// Task가 Latent(비동기)임을 표시
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USG_Task_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ASG_Enemy* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me) return EBTNodeResult::Failed;

	float playTime = Me->ServerRPC_PlayAnimMontage(MontageToPlay);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [&]()
		{
			if (this)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}, playTime, false);

	return EBTNodeResult::InProgress;
}
//
//void USG_Task_PlayMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
//{
//	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
//}
//
//void USG_Task_PlayMontage::OnGameplayTaskDeactivated(UGameplayTask& Task)
//{
//	Super::OnGameplayTaskDeactivated(Task);
//}
//
//void USG_Task_PlayMontage::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
//{
//	
//}
