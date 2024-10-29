// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_SetAimOffsetAlpha.h"
#include "AIController.h"
#include "../SG_Enemy.h"

EBTNodeResult::Type USG_Task_SetAimOffsetAlpha::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ASG_Enemy* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me) return EBTNodeResult::Failed;

	Me->SetAimOffsetAlpha(AimOffsetAlpha);

	return EBTNodeResult::Succeeded;
}
