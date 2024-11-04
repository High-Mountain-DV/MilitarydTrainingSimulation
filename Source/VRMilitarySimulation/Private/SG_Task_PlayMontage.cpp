// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_PlayMontage.h"
#include "AIController.h"
#include "../SG_Enemy.h"
#include "SG_EnemyAnimInstance.h"

USG_Task_PlayMontage::USG_Task_PlayMontage()
{
	NodeName = TEXT("Play Montage (With Notify)");
	// Task가 Latent(비동기)임을 표시
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USG_Task_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ASG_Enemy* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me) return EBTNodeResult::Failed;

	UAnimInstance* Anim = Me->GetMesh()->GetAnimInstance();
	if (!Anim) return EBTNodeResult::Failed;

	// 현재 BT 컴포넌트 저장
	CurrentOwnerComp = &OwnerComp;

	if (bWaitForNotify && !NotifyEndName.IsNone())
	{
		Anim->OnPlayMontageNotifyBegin.AddDynamic(this, &USG_Task_PlayMontage::OnNotifyBegin);
	}
	else
	{
		Anim->OnMontageEnded.AddDynamic(this, &USG_Task_PlayMontage::OnMontageEnded);
	}

	Me->ServerRPC_PlayAnimMontage(MontageToPlay);

	return EBTNodeResult::InProgress;
}

void USG_Task_PlayMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (FinishHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FinishHandle);
	}

	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (ASG_Enemy* Me = Cast<ASG_Enemy>(AIController->GetPawn()))
		{
			if (UAnimInstance* Anim = Me->GetMesh()->GetAnimInstance())
			{
				Anim->OnPlayMontageNotifyEnd.RemoveDynamic(this, &USG_Task_PlayMontage::OnNotifyBegin);
				Anim->OnMontageEnded.RemoveDynamic(this, &USG_Task_PlayMontage::OnMontageEnded);
			}
		}
	}
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void USG_Task_PlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CurrentOwnerComp && Montage == MontageToPlay)
	{
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
	}
}

void USG_Task_PlayMontage::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (CurrentOwnerComp && NotifyName == NotifyEndName)
	{
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
	}
}