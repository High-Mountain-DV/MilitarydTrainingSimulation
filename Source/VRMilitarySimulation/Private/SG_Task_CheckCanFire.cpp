// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Task_CheckCanFire.h"
#include "../SG_EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../SG_Enemy.h"
#include "SG_WeaponMaster.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type USG_Task_CheckCanFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_EnemyAIController* AIController = Cast<ASG_EnemyAIController>(OwnerComp.GetAIOwner());
	check(AIController); if (nullptr == AIController) return EBTNodeResult::Failed;

	ASG_Enemy* Me = Cast<ASG_Enemy>(AIController->GetPawn());
	check(Me); if (nullptr == Me) return EBTNodeResult::Failed;

	ASG_WeaponMaster* Weapon = Me->GetWeapon();
	check(Weapon); if (nullptr == Weapon) return EBTNodeResult::Failed;

	FVector Start = Weapon->FirePosition->GetComponentLocation();
	FVector End = Start + Weapon->FirePosition->GetForwardVector() * 5000;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Me);
	TArray<FHitResult> HitInfos;

	bool HitEnemy = false;

	bool bHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Start, End, FVector(3), FRotator::ZeroRotator, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel7), false, ActorsToIgnore, EDrawDebugTrace::None, HitInfos, false);
	if (bHit)
	{
		for (const FHitResult& hitInfo : HitInfos)
		{
			AActor* hitActor= hitInfo.GetActor();
			if (!hitActor) continue;

			if (hitActor->ActorHasTag(TEXT("Enemy")))
			{	
				HitEnemy = true;
				break;
			}
		}
	}

	EBTNodeResult::Type NodeResult = HitEnemy ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
	return NodeResult;
}
