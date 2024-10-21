// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_MoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

USG_Task_MoveTo::USG_Task_MoveTo()
{
    NodeName = TEXT("Smooth Move To");
    bNotifyTick = true;
}

EBTNodeResult::Type USG_Task_MoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
        return EBTNodeResult::Failed;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
        return EBTNodeResult::Failed;

    // 목표 위치 가져오기
    FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

    // NavMesh 경로 찾기
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
        return EBTNodeResult::Failed;

	/* FPathFindingResult Result = NavSys->FindPathSync(
		 FPathFindingQuery(,
			 AIController,
			 *NavSys,
			 AIPawn->GetActorLocation(),
			 TargetLocation
		 )
	 );*/

	 /* if (!Result.IsSuccessful())
	  {
		  return EBTNodeResult::Failed;
	  }*/

    //CurrentPath = Result.Path;
    CurrentPathPoint = 1; // 첫 번째 경유지점부터 시작
    CurrentSpeed = 0.0f;

    return EBTNodeResult::InProgress;
}

void USG_Task_MoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 목표 위치 가져오기
    FVector TargetLocation;
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
    if (nullptr == TargetActor) 
        TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("LaskKnownLocation"));
    else
        TargetLocation = TargetActor->GetActorLocation();
    FVector CurrentLocation = AIPawn->GetActorLocation();

    // 목표까지의 방향과 거리 계산
    FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
    float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);
    GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("DistanceToTarget: %f"), DistanceToTarget));
    // 목표 지점에 가까워질수록 감속
    float TargetSpeed = MaxSpeed;
    if (DistanceToTarget < 250)
    {
        TargetSpeed = FMath::Lerp(0.0f, MaxSpeed, DistanceToTarget / 250);
    }

    // 가속/감속 처리
    if (CurrentSpeed < TargetSpeed)
    {
        CurrentSpeed += AccelerationRate * DeltaSeconds * MaxSpeed;
    }
    else
    {
        CurrentSpeed -= DecelerationRate * DeltaSeconds * MaxSpeed;
    }
    CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.0f, MaxSpeed);

    // 이동 적용
    AIPawn->AddMovementInput(DirectionToTarget, CurrentSpeed * DeltaSeconds);

    // 목표 지점 도달 체크
    if (DistanceToTarget <= AcceptableRadius)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
