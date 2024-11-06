// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "VRMilitarySimulation.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

ASG_EnemyAIController::ASG_EnemyAIController()
{
	
}

void ASG_EnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComp = GetPerceptionComponent();
	check(PerceptionComp); if (nullptr == PerceptionComp) return;

	PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &ASG_EnemyAIController::OnPerceptionUpdated);
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASG_EnemyAIController::OnTargetPerceptionUpdated);
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
	MyBlackboard = GetBlackboardComponent();
	check(MyBlackboard); if (nullptr == MyBlackboard) return;
	PRINTLOG(TEXT("%s"), *MyBlackboard->GetName());

	MyBlackboard->ClearValue(TEXT("LastKnownLocation"));
}

void ASG_EnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	UE_LOG(LogTemp, Warning, TEXT("UnPossessed"));
}

void ASG_EnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (auto actor : UpdatedActors)
	{
		//PRINTLOG(TEXT("Actor: {%s}"), *actor->GetName());
	}
}

void ASG_EnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasAuthority()) return;

	/*UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
	check(PerceptionSystem); if (nullptr == PerceptionSystem) return;*/

	TSubclassOf<UAISense> AISense = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);

	if (AISense == UAISense_Sight::StaticClass())
	{
		HandleVisualStimuls(Actor, Stimulus);
	}
	else if (AISense == UAISense_Hearing::StaticClass())
	{
		HandleAudioStimuls(Actor, Stimulus);
	}

}

void ASG_EnemyAIController::HandleVisualStimuls(AActor* Actor, FAIStimulus Stimulus)
{
	/*if (Stimulus.WasSuccessfullySensed())
	{
		PRINTLOG(TEXT("성공적으로 타겟 설정"));
		MyBlackboard->SetValueAsObject(TEXT("TargetActor"), Actor);

		if (MyBlackboard->GetValueAsBool(TEXT("bCanFocusOn")))
		{
			SetFocus(Actor);
		}
	}
	else
	{
		PRINTLOG(TEXT("타겟 놓침"));
		auto* TargetActor = MyBlackboard->GetValueAsObject(TEXT("TargetActor"));

	}*/
}

void ASG_EnemyAIController::HandleAudioStimuls(AActor* Actor, FAIStimulus Stimulus)
{
}
