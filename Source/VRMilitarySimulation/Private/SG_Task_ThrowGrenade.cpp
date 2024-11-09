// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_ThrowGrenade.h"
#include "../SG_Enemy.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "SG_Grenade.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "SG_EnemyAnimInstance.h"

USG_Task_ThrowGrenade::USG_Task_ThrowGrenade()
{
	NodeName = TEXT("ThrowGrenade");
}

EBTNodeResult::Type USG_Task_ThrowGrenade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FVector TargetPoint;
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	auto* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	if (!Me) return EBTNodeResult::Failed;

	auto* Anim = Me->GetMesh()->GetAnimInstance();
	if (!Anim) return EBTNodeResult::Failed;

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
			UE_LOG(LogTemp, Warning, TEXT("USG_Task_ThrowGrenade::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetPoint = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}

	AIController->SetFocalPoint(TargetPoint);

	if (bDebugBoxOn)
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), TargetPoint, FVector(15), FColor::Red, FRotator::ZeroRotator, 10);
	
	Me->CurrTask = this;
	Me->TimeMultiplier = TimeMultiplier;
	Me->GrenadeTargetPoint = TargetPoint;

	FVector GrenadeVelocity = ASG_Grenade::GetThrowVelocityToTarget(Me->GetActorLocation(), TargetPoint, TimeMultiplier);
	UE_LOG(LogTemp, Warning, TEXT("Predict GrenadeVelocity: %s"), *GrenadeVelocity.ToString()	);
	if (ASG_Grenade::CheckTrajectoryCollision(GetWorld(), Me->GetActorLocation(), TargetPoint, GrenadeVelocity, Me, bDebugBoxOn))
	{
		UE_LOG(LogTemp, Warning, TEXT("궤적상의 장애물로 인해 수류탄 투척 실패"));
		return EBTNodeResult::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("GrenadeVelocity.size: %f"), GrenadeVelocity.Size());

	Anim->OnPlayMontageNotifyBegin.AddDynamic(Me, &ASG_Enemy::ThrowGrenadeNotifyBegin);
	Anim->OnMontageEnded.AddDynamic(Me, &ASG_Enemy::OnGrenadeMontageEnded);
	
	float MontageChangeThreshold = 1350.0f;
	if (GrenadeVelocity.Size() > MontageChangeThreshold)
		Me->ServerRPC_PlayAnimMontage(EEnemyAnimMontageType::Throw_Grenade);
	else
		Me->ServerRPC_PlayAnimMontage(EEnemyAnimMontageType::Toss_Grenade);

	return NodeResult;
}