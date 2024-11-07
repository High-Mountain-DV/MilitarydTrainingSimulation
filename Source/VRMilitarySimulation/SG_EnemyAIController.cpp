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
	FString output = TEXT("TargetActors: ");
	for (auto targetActor : TargetActors)
	{
		output += targetActor->GetName() + TEXT("  /  ");
	}
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, FString::Printf(TEXT("%s"), *output));
}

void ASG_EnemyAIController::HandleVisualStimuls(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		PRINTLOG(TEXT("성공적으로 타겟 설정, %s"), *Actor->GetName());
		TargetActors.Add(Actor);

		if (nullptr == MyBlackboard->GetValueAsObject(TEXT("TargetActor")))
		{
			MyBlackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
		}
	}
	else
	{
		PRINTLOG(TEXT("타겟 놓침"));
		auto* CurrTargetActor = Cast<AActor>(MyBlackboard->GetValueAsObject(TEXT("TargetActor")));
		check(CurrTargetActor); if (nullptr == CurrTargetActor) return;

		// 제 1타겟으로 지정하고 있던 액터가 사라지면
		if (CurrTargetActor == TargetActors[0])
		{
			TargetActors.RemoveAt(0);

			// 시야내에 적이 없으면
			if (TargetActors.IsEmpty())
			{	
				ClearFocus(EAIFocusPriority::Gameplay);
				MyBlackboard->SetValueAsVector(TEXT("LastKnownLocation"), CurrTargetActor->GetActorLocation());
				MyBlackboard->ClearValue(TEXT("TargetActor"));
			}
			// 다른 적들이 있다면
			else
			{
				MyBlackboard->SetValueAsObject(TEXT("TargetActor"), TargetActors[0]);
			}
		}

		// 우선순위가 낮은 액터가 사라지면
		else
		{
			TargetActors.Remove(CurrTargetActor);
		}
	}
}

void ASG_EnemyAIController::HandleAudioStimuls(AActor* Actor, FAIStimulus Stimulus)
{
	MyBlackboard->SetValueAsVector(TEXT("TargetLocation"), Stimulus.StimulusLocation);
	PRINTLOG(TEXT("소리 들림"));
}
