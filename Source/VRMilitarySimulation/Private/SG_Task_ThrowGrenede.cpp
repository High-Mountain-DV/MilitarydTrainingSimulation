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

USG_Task_ThrowGrenede::USG_Task_ThrowGrenede()
{
	NodeName = TEXT("ThrowGrenede");
	//bNotifyTick = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMontage(TEXT("/Script/Engine.AnimMontage'/Game/MilitarySimulator/JSG/Animations/Weapon/AM_Grenede_Throw.AM_Grenede_Throw'"));
	if (tempMontage.Succeeded())
	{
		AM_Throw_Grenede = tempMontage.Object;
	}
	check(AM_Throw_Grenede);
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

	Anim = Me->GetMesh()->GetAnimInstance();
	if (!Anim) return EBTNodeResult::Failed;

	UE_LOG(LogTemp, Warning, TEXT("Anim: %s"), *Anim->GetName());


	// 현재 BT 컴포넌트 저장
	CurrentOwnerComp = &OwnerComp;

	//Anim->OnPlayMontageNotifyBegin.AddDynamic(this, &USG_Task_ThrowGrenede::GrabGrenedeAtRightHandNotifyBegin);
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


	FVector GrenedeVelocity = GetThrowVelocityToTarget(TargetPoint);
	if (CheckTrajectoryCollision(TargetPoint, GrenedeVelocity))
	{
		UE_LOG(LogTemp, Warning, TEXT("궤적상의 장애물로 인해 수류탄 투척 실패"));
		return EBTNodeResult::Failed;
	}

	Me->ServerRPC_PlayAnimMontage(AM_Throw_Grenede);

	return NodeResult;
}

FVector USG_Task_ThrowGrenede::GetThrowVelocityToTarget(const FVector& TargetLocation)
{
	FVector ret;

	FVector CurLocation = Me->GetActorLocation();
	FVector Sub = TargetLocation - CurLocation;
	Sub.Z = 0;

	FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, TargetLocation);

	// Horizontal Distance
	float S = Sub.Length();
	// Time
	float t = (S / 1000) * TimeMultiplier;
	// Gravity
	float g = -490;
	// Vertical Component of velocity
	float v = -t * g * 0.5;

	// Horizontal Component of velocity
	float u = (S - (0.5 * g * FMath::Square(t))) / t;

	ret.X = DirectionUnitVector.X * u;
	ret.Y = DirectionUnitVector.Y * u;
	ret.Z = v;


	//ret += FVector(FMath::RandRange(MinNoise, MaxNoise), FMath::RandRange(MinNoise, MaxNoise), FMath::RandRange(MinNoise, MaxNoise));
	//ret*= FMath::RandRange(0.8, 1.0);
	return ret;
}

bool USG_Task_ThrowGrenede::CheckTrajectoryCollision(const FVector& TargetLocation, const FVector& Velocity)
{
	//FVector StartPos = Me->GetActorLocation();
	//// 가상의 물리 객체 생성
	//FPredictProjectilePathParams PredictParams(
	//	10.0f,             // 발사체 반지름
	//	StartPos,          // 시작 위치
	//	Velocity,          // 초기 속도
	//	3.0f,             // 최대 시뮬레이션 시간
	//	ECollisionChannel::ECC_GameTraceChannel9,  // 충돌 채널
	//	Me                 // 무시할 액터
	//);

	//PredictParams.bTraceComplex = true;
	//PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	//PredictParams.DrawDebugTime = 5.0f;

	//FPredictProjectilePathResult PredictResult;

	//// 언리얼 엔진의 물리 기반 예측 사용
	//bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);

	//if (bHit)
	//{
	//	float dist = FVector::Dist(PredictResult.HitResult.ImpactPoint, TargetLocation);
	//	if (dist >= 500.0f)
	//	{
	//		return true; // 장애물 있음
	//	}
	//}

	return false;
}

void USG_Task_ThrowGrenede::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMontageEnded"));
	if (CurrentOwnerComp && Montage == AM_Throw_Grenede)
	{
		check(Anim); if (nullptr == Anim) return;
		
		//Anim->OnMontageEnded.RemoveDynamic(this, &USG_Task_ThrowGrenede::OnMontageEnded);
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
	}
}

void USG_Task_ThrowGrenede::GrabGrenedeAtRightHandNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (CurrentOwnerComp && NotifyName == TEXT("GrenedeAtRightHand"))
	{
		check(Anim); if (nullptr == Anim) return;
		UE_LOG(LogTemp, Warning, TEXT("GrabGrenedeAtRightHandNotifyBegin"));
		//Anim->OnPlayMontageNotifyEnd.RemoveDynamic(this, &USG_Task_ThrowGrenede::GrabGrenedeAtRightHandNotifyBegin);
	}
}

void USG_Task_ThrowGrenede::ThrowGrenedeNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UE_LOG(LogTemp, Warning, TEXT("ThrowGrenedeNotifyBegin"));
	if (CurrentOwnerComp && NotifyName == TEXT("ThrowGrenede"))
	{
		check(Anim); if (nullptr == Anim) return;

		auto* Grenede = Me->Grenede;
		if (nullptr == Grenede) return;

		Grenede->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Grenede->SetPhysicalOption();
		Grenede->Active(Me);

		Grenede->Throw(TargetPoint);
		//Anim->OnPlayMontageNotifyEnd.RemoveDynamic(this, &USG_Task_ThrowGrenede::ThrowGrenedeNotifyBegin);
	}
}
