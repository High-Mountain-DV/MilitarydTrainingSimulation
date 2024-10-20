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

	FVector TargetActorLocation;

	auto* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));
	if (nullptr == TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("타겟 액터 소실, 마지막 추적 위치로 사격"));
		TargetActorLocation = Blackboard->GetValueAsVector(TEXT("LastKnownLocation"));
	}
	else
	{
		TargetActorLocation = TargetActor->GetActorLocation();
	}
	TargetActorLocation.Z += 30;
	ControlledPawn->Aim(TargetActorLocation);

	return EBTNodeResult::Succeeded;
}
