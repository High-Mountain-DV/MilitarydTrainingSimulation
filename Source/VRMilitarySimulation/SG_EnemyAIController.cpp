// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"

void ASG_EnemyAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn /*= true*/)
{
	Super::UpdateControlRotation(DeltaTime, false);

	if (bUpdatePawn)
	{
		APawn* const Me = GetPawn();

		const FRotator CurrentPawnRotation = Me->GetActorRotation();

		SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(CurrentPawnRotation, ControlRotation, DeltaTime, SmoothFocusInterpSpeed);

		if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
		{
			Me->FaceRotation(SmoothTargetRotation, DeltaTime);
		}
	}
}

void ASG_EnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//RunBehaviorTree(BT_Enemy);
}
