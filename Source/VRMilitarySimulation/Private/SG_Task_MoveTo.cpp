// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_MoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "../SG_Enemy.h"
#include "../VRMilitarySimulation.h"
#include "NavigationPath.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USG_Task_MoveTo::USG_Task_MoveTo()
{
	NodeName = TEXT("Smooth Move To");
	bNotifyTick = true;
}

EBTNodeResult::Type USG_Task_MoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//UE_LOG(LogTemp, Warning, TEXT("USG_Task_MoveTo::ExecuteTask"));
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
		UE_LOG(LogTemp, Warning, TEXT("이미 접근, MoveTo 노드 중단"));
		return EBTNodeResult::Succeeded;
	}
	bool FindResult = FindPathPoints(Me);

	if (!FindResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindPathPoints(%s) is Failed"), *TargetLocation.ToString());
		return EBTNodeResult::Failed;
	}

	if (bRemoveTargetLocationKey)
	{
		Me->SetTargetLocationKeyName(BlackboardKey.GetSelectedKeyID());
	}

	Me->bAutoMoveActive = false;

	return EBTNodeResult::InProgress;
}

void USG_Task_MoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return ;

	auto* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me)
		return ;
	if (Me->bDebugMoveTask)
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), Me->NextTargetLocation, FVector(30), FColor::Purple, FRotator::ZeroRotator, DeltaSeconds);
	FVector DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(Me->GetActorLocation(), Me->NextTargetLocation);

	if (Me->bFaceToDirection)
	{
		// 현재 액터의 rotation을 구함
		FRotator CurrentRotation = Me->GetActorRotation();

		// DirectionVector를 회전값으로 변환
		FRotator TargetRotation = DirectionVector.Rotation();

		// 현재 회전값에서 목표 회전값으로 부드럽게 보간
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,    // 현재 회전값
			TargetRotation,     // 목표 회전값
			DeltaSeconds,         // 델타 타임
			2.0f              // 회전 속도 (이 값을 조절하여 회전 속도 변경)
		);
		// 새로운 회전값 적용
		NewRotation.Pitch = 0;
		Me->SetActorRotation(NewRotation);
	}
	
	if (!bRun && FVector::Dist(Me->GetActorLocation(), Me->TargetLocation) <= 75 + Me->AcceptableRadius)
	{
		Me->SpeedScale = FMath::Max(0.3, Me->SpeedScale - 0.025);
	}
	
	Me->AddMovementInput(DirectionVector, Me->SpeedScale);
	if (Me->GetVelocity().Equals(FVector(0), 100))
	{
		Me->ZeroVelocityCount++;
		if (Me->ZeroVelocityCount >= 60)
		{
			float randomRangeValue = 200;
			float delta_x = FMath::RandRange(-randomRangeValue, randomRangeValue);
			float delta_y = FMath::RandRange(-randomRangeValue, randomRangeValue);

			Me->NextTargetLocation.X = Me->GetActorLocation().X + delta_x;
			Me->NextTargetLocation.Y = Me->GetActorLocation().Y + delta_y;

			if (false == Me->bCanMove) Me->PointIndex -= 1;
			Me->bCanMove = true;

			Me->ZeroVelocityCount = 0;
			if (Me->bDebugMoveTask)
				UKismetSystemLibrary::DrawDebugBox(GetWorld(), Me->NextTargetLocation, FVector(30), FColor::Purple, FRotator::ZeroRotator, DeltaSeconds);
		}
	}
	if (ASG_Enemy::ArriveAtLocation(Me->GetActorLocation(), Me->NextTargetLocation, Me->TempAcceptableRadius))
	{
		Me->PointIndex += 1;
		Me->ZeroVelocityCount = 0;
		Me->bCanMove = false;
		if (Me->PointIndex < Me->PathPoints.Num())
		{
			Me->NextTargetLocation = Me->PathPoints[Me->PointIndex];
			if (Me->PointIndex == Me->PathPoints.Num() - 1)
			{
				Me->TempAcceptableRadius = AcceptableRadius;
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("TempAcceptableRadius Set! : %f"), AcceptableRadius));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Arrive at location"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

bool USG_Task_MoveTo::FindPathPoints(ASG_Enemy* Me)
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

void USG_Task_MoveTo::DebugPoints(const TArray<FVector>& Array)
{
	for (const FVector& point : Array)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, FVector(10), FColor::Blue, FRotator::ZeroRotator, 10);
	}
}

void USG_Task_MoveTo::InitEnemyVariables(class ASG_Enemy* Me)
{
	Me->TargetLocation = FVector::ZeroVector;

	Me->SpeedScale = bRun ? 1.0f : 0.75f;
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
