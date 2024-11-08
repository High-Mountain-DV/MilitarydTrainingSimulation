// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_ThrowGrenede.h"
#include "../SG_Enemy.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "SG_Grenede.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "SG_EnemyAnimInstance.h"

USG_Task_ThrowGrenede::USG_Task_ThrowGrenede()
{
	NodeName = TEXT("ThrowGrenede");
	//bNotifyTick = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> tempThrowMontage(TEXT("/Script/Engine.AnimMontage'/Game/MilitarySimulator/JSG/Animations/Weapon/AM_Grenede_Throw.AM_Grenede_Throw'"));
	if (tempThrowMontage.Succeeded())
	{
		AM_Throw_Grenede = tempThrowMontage.Object;
	}

	ConstructorHelpers::FObjectFinder<UAnimMontage>tempTossMontage(TEXT("/Script/Engine.AnimMontage'/Game/MilitarySimulator/JSG/Animations/Weapon/AM_Grenede_Toss.AM_Grenede_Toss'"));
	if (tempTossMontage.Succeeded())
	{
		AM_Toss_Grenede = tempTossMontage.Object;
	}
	check(AM_Throw_Grenede);
	check(AM_Toss_Grenede);
}

EBTNodeResult::Type USG_Task_ThrowGrenede::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("USG_Task_ThrowGrenede::ExecuteTask"));
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me) return EBTNodeResult::Failed;

	Anim = Cast<USG_EnemyAnimInstance>( Me->GetMesh()->GetAnimInstance());
	if (!Anim) return EBTNodeResult::Failed;

	UE_LOG(LogTemp, Warning, TEXT("Anim: %s"), *Anim->GetName());


	// 현재 BT 컴포넌트 저장
	CurrentOwnerComp = &OwnerComp;

	Anim->OnPlayMontageNotifyBegin.AddDynamic(this, &USG_Task_ThrowGrenede::ThrowGrenedeNotifyBegin);
	Anim->OnMontageEnded.AddDynamic(this, &USG_Task_ThrowGrenede::OnMontageEnded);

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(KeyValue);
		if (TargetActor)
		{
			TargetPoint = TargetActor->GetActorLocation();
		}
		else
		{
			TargetPoint = MyBlackboard->GetValueAsVector(TEXT("LaskKnownLocation"));
			UE_LOG(LogTemp, Warning, TEXT("USG_Task_ThrowGrenede::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetPoint = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}

	if (bDebugBoxOn)
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), TargetPoint, FVector(15), FColor::Red, FRotator::ZeroRotator, 10);

	FVector GrenedeVelocity = GetThrowVelocityToTarget(TargetPoint);
	UE_LOG(LogTemp, Warning, TEXT("GrenedeVelocity: %s"), *GrenedeVelocity.ToString()	);
	if (CheckTrajectoryCollision(TargetPoint, GrenedeVelocity))
	{
		UE_LOG(LogTemp, Warning, TEXT("궤적상의 장애물로 인해 수류탄 투척 실패"));
		return EBTNodeResult::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("GrenedeVelocity.size: %f"), GrenedeVelocity.Size());
	if (GrenedeVelocity.Size() > 850)
		Me->ServerRPC_PlayAnimMontage(AM_Throw_Grenede);
	else
		Me->ServerRPC_PlayAnimMontage(AM_Toss_Grenede);

	return NodeResult;
}

FVector USG_Task_ThrowGrenede::GetThrowVelocityToTarget(const FVector& TargetLocation)
{
	// 1. 입력 유효성 검사
	if (!Me && !Grenede)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid actor references"));
		return FVector::ZeroVector;
	}

	// 2. 시작 위치 결정
	FVector CurLocation = Grenede ? Grenede->GetActorLocation() : Me->GetActorLocation();

	// 3. 전체 변위 계산 (높이 차이 포함)
	FVector Displacement = TargetLocation - CurLocation;
	float TotalDistance = Displacement.Size();

	// 4. 최소 거리 체크
	const float MinDistance = 10.0f;
	if (TotalDistance < MinDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target too close, distance: %f"), TotalDistance);
		return FVector::ZeroVector;
	}

	// 5. 수평 거리와 높이 차이 계산
	float HorizontalDistance = FVector(Displacement.X, Displacement.Y, 0.0f).Size();
	float HeightDifference = Displacement.Z;

	// 6. 물리 상수
	const float Gravity = -980.0f;  // 실제 중력 가속도 (cm/s^2)

	// 7. 시간 계산 개선
	float EstimatedTime;
	if (TimeMultiplier > 0)
	{
		// 거리에 따른 비선형 시간 계산
		EstimatedTime = FMath::Sqrt(2.0f * HorizontalDistance / FMath::Abs(Gravity)) * TimeMultiplier;
		EstimatedTime = FMath::Clamp(EstimatedTime, 0.1f, 10.0f);  // 합리적인 범위로 제한
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid TimeMultiplier"));
		return FVector::ZeroVector;
	}

	// 8. 속도 계산
	float VerticalVelocity;
	float HorizontalVelocity;

	// 수직 속도 계산 (높이 차이 고려)
	VerticalVelocity = (HeightDifference - (0.5f * Gravity * EstimatedTime * EstimatedTime)) / EstimatedTime;

	// 수평 속도 계산
	HorizontalVelocity = HorizontalDistance / EstimatedTime;


	// 9. 최종 속도 벡터 계산
	FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, TargetLocation);
	FVector HorizontalVelocityVector = FVector(
		DirectionUnitVector.X * HorizontalVelocity,
		DirectionUnitVector.Y * HorizontalVelocity,
		VerticalVelocity
	);

	// 10. 속도 제한
	const float MaxVelocity = 10000.0f;  // 최대 속도 제한 (cm/s)
	if (HorizontalVelocityVector.Size() > MaxVelocity)
	{
		UE_LOG(LogTemp, Warning, TEXT("Velocity exceeded maximum limit, clamping"));
		HorizontalVelocityVector = HorizontalVelocityVector.GetClampedToSize(0.0f, MaxVelocity);
	}

	// 11. 디버그 정보
	UE_LOG(LogTemp, Log, TEXT("Throw Calculation: Distance=%f, Time=%f, Velocity=%s"),
		TotalDistance, EstimatedTime, *HorizontalVelocityVector.ToString());

	HorizontalVelocityVector *= FMath::RandRange(0.8f, 0.95f);

	return HorizontalVelocityVector;
}

bool USG_Task_ThrowGrenede::CheckTrajectoryCollision(const FVector& TargetLocation, const FVector& Velocity)
{
	FVector StartPos = Me->GetActorLocation();
	// 가상의 물리 객체 생성
	FPredictProjectilePathParams PredictParams(
		3.0f,             // 발사체 반지름
		StartPos,          // 시작 위치
		Velocity,          // 초기 속도
		3.0f,             // 최대 시뮬레이션 시간
		ECollisionChannel::ECC_GameTraceChannel9,  // 충돌 채널
		Me                 // 무시할 액터
	);

	PredictParams.bTraceComplex = false;
	if (bDebugBoxOn)
		PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	else 
		PredictParams.DrawDebugType = EDrawDebugTrace::None;
	PredictParams.DrawDebugTime = 2.0f;
	FPredictProjectilePathResult PredictResult;

	// 언리얼 엔진의 물리 기반 예측 사용
	bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	FString HitActorName;
	FString HitComponentName;
	if (PredictResult.HitResult.GetActor())
	{
		bool bGroundHit = PredictResult.HitResult.GetActor()->ActorHasTag(TEXT("Ground"));
		if (bGroundHit) return false;
		HitActorName = PredictResult.HitResult.GetActor()->GetName();
	}
	if (PredictResult.HitResult.GetComponent())
		HitComponentName = PredictResult.HitResult.GetComponent()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("HitActorName: %s, HitComponentName: %s"), *HitActorName, *HitComponentName);
	if (bHit)
	{
		float dist = FVector::Dist(PredictResult.HitResult.ImpactPoint, TargetLocation);
		if (dist >= 500.0f)
		{
			return true; // 장애물 있음
		}
	}

	return false;
}

void USG_Task_ThrowGrenede::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMontageEnded"));
	if (CurrentOwnerComp && (Montage == AM_Throw_Grenede || Montage == AM_Toss_Grenede))
	{
		check(Anim); if (nullptr == Anim) return;
		
		Anim->OnMontageEnded.RemoveDynamic(this, &USG_Task_ThrowGrenede::OnMontageEnded);
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
	}
}

void USG_Task_ThrowGrenede::GrabGrenedeAtRightHandNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (CurrentOwnerComp && NotifyName == TEXT("GrenedeAtRightHand"))
	{
		check(Anim); if (nullptr == Anim) return;
		UE_LOG(LogTemp, Warning, TEXT("GrabGrenedeAtRightHandNotifyBegin"));
		Anim->OnPlayMontageNotifyEnd.RemoveDynamic(this, &USG_Task_ThrowGrenede::GrabGrenedeAtRightHandNotifyBegin);
	}
}

void USG_Task_ThrowGrenede::ThrowGrenedeNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UE_LOG(LogTemp, Warning, TEXT("ThrowGrenedeNotifyBegin"));
	if (CurrentOwnerComp && NotifyName == TEXT("ThrowGrenede"))
	{
		check(Anim); if (nullptr == Anim) return;

		Grenede = Me->Grenede;
		if (nullptr == Grenede) return;

		Grenede->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Grenede->SetPhysicalOption();
		Grenede->Active(Me);

		FVector Velocity = GetThrowVelocityToTarget(TargetPoint);
		UE_LOG(LogTemp, Warning, TEXT("Real Velocity: %s"), *Velocity.ToString());
		CheckTrajectoryCollision(TargetPoint, Velocity);
		Grenede->BaseMesh->AddAngularImpulseInDegrees(FVector(FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f)));
		Grenede->BaseMesh->AddImpulse(Velocity, NAME_None, true);
		//Grenede->Throw(TargetPoint);
		UE_LOG(LogTemp, Warning, TEXT("Anim: %s"), *Anim->GetName());
		Anim->OnPlayMontageNotifyBegin.RemoveDynamic(this, &USG_Task_ThrowGrenede::ThrowGrenedeNotifyBegin);
	}
}
