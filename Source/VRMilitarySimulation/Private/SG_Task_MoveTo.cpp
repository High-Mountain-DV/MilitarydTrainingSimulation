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
	UE_LOG(LogTemp, Warning, TEXT("USG_Task_MoveTo::ExecuteTask"));
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
    EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
        return EBTNodeResult::Failed;

    AIPawn = Cast<ASG_Enemy>(AIController->GetPawn());
    if (!AIPawn)
        return EBTNodeResult::Failed;

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
	float OutDist;
	if (ArriveAtLocation(TargetLocation, OutDist))
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 접근"));
		return EBTNodeResult::Succeeded;
	}
	bool FindResult = FindPathPoints();

	if (!FindResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindPathPoints(%s) is Failed"), *TargetLocation.ToString());
		return EBTNodeResult::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("TargetLocation: %s"), *TargetLocation.ToString());

    return EBTNodeResult::InProgress;
}

void USG_Task_MoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (StartMovement)
	{
		DirectionVector = GetDirectionToTarget();

		// 현재 액터의 rotation을 구합니다
		FRotator CurrentRotation = AIPawn->GetActorRotation();

		// DirectionVector를 회전값으로 변환
		FRotator TargetRotation = DirectionVector.Rotation();

		// 현재 회전값에서 목표 회전값으로 부드럽게 보간
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,    // 현재 회전값
			TargetRotation,     // 목표 회전값
			DeltaSeconds,         // 델타 타임
			5.0f              // 회전 속도 (이 값을 조절하여 회전 속도 변경)
		);

		// 새로운 회전값 적용
		AIPawn->SetActorRotation(NewRotation);
		if (bCloseToTargetLocation)	SpeedScale = FMath::Max(0.5, SpeedScale	 - 0.01);

		AIPawn->DebugArrow->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(DirectionVector));
		UE_LOG(LogTemp, Warning, TEXT("Speed: {%f}"), SpeedScale);
		//UE_LOG(LogTemp, Warning, TEXT("DirectionVector: {%s}, Speed: {%f}, Velocity: {%s}"), *DirectionVector.ToString(), SpeedScale, *AIPawn->GetVelocity().ToString());
		AIPawn->AddMovementInput(DirectionVector, SpeedScale);
		if (AIPawn->GetVelocity().Equals(FVector(0), 100))
		{
			ZeroVelocityCount++;
			if (ZeroVelocityCount >= 60)
			{
				float delta_x = FMath::RandRange(-100, 100);
				float delta_y = FMath::RandRange(-100, 100);

				NextTargetLocation.X += delta_x;
				NextTargetLocation.Y += delta_y;
				UKismetSystemLibrary::DrawDebugBox(GetWorld(), NextTargetLocation, FVector(15), FColor::Red, FRotator::ZeroRotator, 10);
				ZeroVelocityCount = 0;
			}


		}

		float OutDist;
		if (ArriveAtLocation(NextTargetLocation, OutDist))
		{
			PointIndex += 1;
			ZeroVelocityCount = 0;
			if (PointIndex < PathPoints.Num())
			{
				NextTargetLocation = PathPoints[PointIndex];
				StartMovement = true;
				// 나중에 도착지에 가까워지면 speed 줄이는 코드 넣기
				float dist = FVector::Distance(AIPawn->GetActorLocation(), FVector(NextTargetLocation.X, NextTargetLocation.Y, AIPawn->GetActorLocation().Z));
				if (PointIndex == PathPoints.Num() - 1 && dist <= AcceptableRadius + 50)
				{
					bCloseToTargetLocation = true;
				}
				// 나중에 도착지에 가까워지면 speed 줄이는 코드 넣기
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Arrive at location"));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				//StopMovement();
			}
		}
	}
}

bool USG_Task_MoveTo::FindPathPoints()
{
	PRINTLOG(TEXT(""));
	PointIndex = 1;
	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), AIPawn->GetActorLocation(), TargetLocation);
	if (!Path)
	{
		UE_LOG(LogTemp, Warning, TEXT("!Path"));
		return false;
	}

	PathPoints = Path->PathPoints;
	if (PathPoints.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path is not exist"));
		return false;
	}
	NextTargetLocation = PathPoints[PointIndex];
	if (PathFindDebug)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), NextTargetLocation, FVector(15), FColor::Purple, FRotator::ZeroRotator, 10);
	}
	SpeedScale = 1;
	StartMovement = true;
	if (PathFindDebug)
	{
		DebugPoints(PathPoints);
	}
	return true;
}

void USG_Task_MoveTo::DebugPoints(const TArray<FVector>& Array)
{
	for (const FVector& point : Array)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, FVector(10), FColor::Blue, FRotator::ZeroRotator, 1000);
	}
}

FVector USG_Task_MoveTo::GetDirectionToTarget()
{
	FVector fromLocation = AIPawn->GetActorLocation();
	FVector ToLocation = FVector(NextTargetLocation.X, NextTargetLocation.Y, fromLocation.Z);

	return UKismetMathLibrary::GetDirectionUnitVector(fromLocation, ToLocation);
}


bool USG_Task_MoveTo::ArriveAtLocation(FVector EndLocation, float& OutDist)
{
	OutDist = FVector::Distance(AIPawn->GetActorLocation(), FVector(EndLocation.X, EndLocation.Y, AIPawn->GetActorLocation().Z));
	return (OutDist <= AcceptableRadius);
}

EBTNodeResult::Type USG_Task_MoveTo::StopMovement()
{
	SpeedScale = 0;
	StartMovement = 0;
	DirectionVector = AIPawn->GetActorForwardVector();

	return EBTNodeResult::Succeeded;
}
