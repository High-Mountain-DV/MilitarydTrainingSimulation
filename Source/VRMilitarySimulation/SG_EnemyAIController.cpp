// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "VRMilitarySimulation.h"

ASG_EnemyAIController::ASG_EnemyAIController()
{
	
}

void ASG_EnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComp = GetPerceptionComponent();
	check(PerceptionComp); if (nullptr == PerceptionComp) return;

	PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &ASG_EnemyAIController::OnPerceptionUpdated);
}

void ASG_EnemyAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

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

	if(HasAuthority())	RunBehaviorTree(BT_Enemy);
}

void ASG_EnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	UE_LOG(LogTemp, Warning, TEXT("UnPossessed"));
}

void ASG_EnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	PRINTLOG(TEXT(""));

	for (auto actor : UpdatedActors)
	{
		PRINTLOG(TEXT("Actor: {%s}"), *actor->GetName());
	}
}
