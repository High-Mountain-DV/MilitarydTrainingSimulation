// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_BurstFire.h"
#include "AIController.h"
#include "../SG_Enemy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
EBTNodeResult::Type USG_Task_BurstFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerController = OwnerComp.GetAIOwner();
	ControlledPawn = Cast<ASG_Enemy>(OwnerController->GetPawn());

	if (!ControlledPawn->Fire())
	{
		auto* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(ControlledPawn);
		Blackboard->SetValueAsBool(TEXT("bMagazineEmpty"), true);
	}
	return EBTNodeResult::Succeeded;
}
