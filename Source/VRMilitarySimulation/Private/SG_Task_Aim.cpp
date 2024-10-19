// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_Aim.h"
#include "AIController.h"
#include "../SG_Enemy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
EBTNodeResult::Type USG_Task_Aim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerController = OwnerComp.GetAIOwner();
	ControlledPawn = Cast<ASG_Enemy>(OwnerController->GetPawn());

	auto* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(ControlledPawn);
	check(Blackboard); if (nullptr == Blackboard) return EBTNodeResult::Failed;

	auto* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));
	check(TargetActor); if (nullptr == TargetActor) return EBTNodeResult::Failed;

	ControlledPawn->Aim(TargetActor);

	return EBTNodeResult::Succeeded;
}
