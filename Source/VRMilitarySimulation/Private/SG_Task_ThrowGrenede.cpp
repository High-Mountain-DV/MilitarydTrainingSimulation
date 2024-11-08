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
	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), TargetLocation, 20, 20, FRotator::ZeroRotator, FColor::Red, 10, 1);

	FVector StartPos = Me->GetActorLocation();
	FVector CurrVelocity = Velocity;
	const float TimeStep = 0.1f;  // 0.1초 간격으로 체크
	const float MaxTime = (TargetLocation - StartPos).Size() / Velocity.Size(); // 예상 도착 시간
	const float G = -980;
	const float AirResistanceCoeff = 0.01f; // 공기 저항 계수
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Me);
	for (float Time = 0; Time <= MaxTime; Time += TimeStep)
	{
		// 포물선 상의 점 계산
		FVector Position = StartPos + CurrVelocity * Time;
		Position.Z += G * Time * Time * 0.5f;  // 중력 적용

		// Velocity 감소 (공기 저항 및 중력 적용)
		CurrVelocity = CurrVelocity + FVector(0, 0, G * TimeStep); // 중력 가속도 적용
		CurrVelocity = CurrVelocity - CurrVelocity.GetSafeNormal() * CurrVelocity.Size() * AirResistanceCoeff * TimeStep; // 공기 저항 감속

		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartPos, Position, FColor::Purple, 10, 2);
		// 장애물 체크
		FHitResult HitResult;
		// 이전 위치에서 현재 위치까지 LineTrace
		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartPos, Position, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel9), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Red, FColor::Purple, 5);
		/*bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartPos,
			Position,
			ECC_WorldStatic
		);*/

		if (bHit)
		{
			// 장애물이 있다면 true 반환
			UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), HitResult.ImpactPoint, 15, 15, FRotator::ZeroRotator, FColor::Yellow, 10, 1);
			//UE_LOG(LogTemp, Warning, TEXT("OutHitActor: %s, OutHitComponent: %s"), *HitResult.GetActor()->GetName(), *HitResult.GetComponent()->GetName());
			float dist = FVector::Dist(HitResult.ImpactPoint, TargetLocation);
			if (dist >= 100) 
			{
				UE_LOG(LogTemp, Warning, TEXT("dist가 %f이라 장애물로 판정"), dist);
				return true;
			}
		}

		StartPos = Position; // 다음 체크를 위해 시작점 업데이트
	}

	return false; // 장애물이 없으면 false 반환
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
