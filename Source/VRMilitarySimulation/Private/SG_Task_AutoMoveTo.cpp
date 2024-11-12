// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_AutoMoveTo.h"
#include "../SG_Enemy.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

USG_Task_AutoMoveTo::USG_Task_AutoMoveTo()
{
	NodeName = TEXT("AutoMoveTo");

	// accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USG_Task_AutoMoveTo, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USG_Task_AutoMoveTo, BlackboardKey));
}
void USG_Task_AutoMoveTo::InitEnemyVariables(ASG_Enemy* Me)
{
	Me->TargetLocation = FVector::ZeroVector;

	if (MovementType == EEnemyMovementType::Run)
	{
		Me->SpeedScale = 1.0f;
	}
	else if (MovementType == EEnemyMovementType::Walk)
	{
		Me->SpeedScale = 0.6f;
	}
	else
	{
		Me->SpeedScale = 0.4f;
	}
	Me->ZeroVelocityCount = 0;
	Me->bCanMove = true;
	Me->PointIndex = 1;
	Me->PathPoints.Empty();
	Me->NextTargetLocation = FVector::ZeroVector;
	Me->AcceptableRadius = AcceptableRadius;
	Me->TempAcceptableRadius = 50;

	//테스트를 위해 잠시 Debug를 무조건 True로 설정
	//Me->bDebugMoveTask = bDebugBoxOn;
	Me->bDebugMoveTask = true;
	Me->bFaceToDirection = bFaceToDirection;
}
EBTNodeResult::Type USG_Task_AutoMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	auto* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me)
		return EBTNodeResult::Failed;

	InitEnemyVariables(Me);

	FVector TargetLocation;

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(KeyValue);
		if (TargetActor)
		{
			TargetLocation = TargetActor->GetActorLocation();
		}
		else
		{
			TargetLocation = MyBlackboard->GetValueAsVector(TEXT("LaskKnownLocation"));
			UE_LOG(LogTemp, Warning, TEXT("USG_Task_MoveTo::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector Location = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		TargetLocation = Location;
	}
	Me->TargetLocation = TargetLocation;
	if (Me->bDebugMoveTask)
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), Me->TargetLocation, FVector(30), FColor::Red, FRotator(45, 45, 0), 7.5f);
	if (ASG_Enemy::ArriveAtLocation(Me->GetActorLocation(), Me->TargetLocation, Me->AcceptableRadius))
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 접근, AutoMoveTo 노드 중단"));
		return EBTNodeResult::Succeeded;
	}
	bool FindResult = FindPathPoints(Me);

	if (!FindResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindPathPoints(%s) is Failed"), *TargetLocation.ToString());
		return EBTNodeResult::Failed;
	}

	Me->bAutoMoveActive = true;
	return EBTNodeResult::Succeeded;
}

bool USG_Task_AutoMoveTo::FindPathPoints(ASG_Enemy* Me)
{
	Me->PointIndex = 1;
	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), Me->GetActorLocation(), Me->TargetLocation);
	if (!Path)
	{
		UE_LOG(LogTemp, Warning, TEXT("!Path"));
		return false;
	}

	Me->PathPoints = Path->PathPoints;
	if (Me->PathPoints.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path is not exist"));
		return false;
	}
	Me->NextTargetLocation = Me->PathPoints[Me->PointIndex];
	if (Me->bDebugMoveTask)
	{
		DebugPoints(Me->PathPoints);
	}

	if (Me->PathPoints.Num() == 2) Me->TempAcceptableRadius = Me->AcceptableRadius;
	return true;
}

void USG_Task_AutoMoveTo::DebugPoints(const TArray<FVector>& Array)
{
	for (const FVector& point : Array)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, FVector(10), FColor::Blue, FRotator::ZeroRotator, 10);
	}
}