// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "VRMilitarySimulation.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SG_Enemy.h"
#include "Kismet/KismetSystemLibrary.h"

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

	Me = Cast<ASG_Enemy>(InPawn);
	check(Me); if (nullptr == Me) return;

	if(HasAuthority())	
	{
		RunBehaviorTree(BT_Enemy);
		//Me->EnemyAIController = this;
		//Me->BehaviorTreeComp = Cast<UBehaviorTreeComponent>(BrainComponent);
		
		MyBlackboard = GetBlackboardComponent();
		check(MyBlackboard); if (nullptr == MyBlackboard) return;
		PRINTLOG(TEXT("%s"), *MyBlackboard->GetName());

		MyBlackboard->ClearValue(TEXT("LastKnownLocation"));
	}
}

void ASG_EnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	//UE_LOG(LogTemp, Warning, TEXT("UnPossessed"));
}

void ASG_EnemyAIController::SetBehaviorTreeComponent(class UBehaviorTreeComponent* NewComp)
{
	Me->SetBehaviorTreeComponent(NewComp);
	
}

void ASG_EnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	//PRINTLOG(TEXT("UpdatedActors.SIze: %d"), UpdatedActors.Num());
	//// 모든 감지된 타겟들을 새로 리스트에 넣음
	//TArray<AActor*> NewTargetActors;

	//for (AActor* Actor : UpdatedActors)
	//{
	//	PRINTLOG(TEXT("Actor: %s"), *Actor->GetName())
	//	if (Actor) // 적인지 확인
	//	{
	//		FAIStimulus Stimulus;
	//		FActorPerceptionBlueprintInfo Info;
	//		GetAIPerceptionComponent()->GetActorsPerception(Actor, Info);
	//		FAIStimulus LastSensedStimuli = Info.LastSensedStimuli.Num() > 0 ? Info.LastSensedStimuli[0] : FAIStimulus();
	//		if (LastSensedStimuli.WasSuccessfullySensed())
	//		{
	//			NewTargetActors.Add(Actor); // 감지된 타겟을 새 타겟 리스트에 추가
	//			PRINTLOG(TEXT("Add Target: %s"), *Actor->GetName());
	//		}
	//	}
	//}

	//// TargetActors 리스트 업데이트
	//TargetActors = NewTargetActors;

	//// 첫 번째 타겟을 블랙보드에 설정
	//if (TargetActors.Num() > 0)
	//{
	//	MyBlackboard->SetValueAsObject(TEXT("TargetActor"), TargetActors[0]);
	//}
	//else
	//{
	//	// 타겟이 없으면 블랙보드에서 삭제
	//	MyBlackboard->ClearValue(TEXT("TargetActor"));
	//	ClearFocus(EAIFocusPriority::Gameplay);
	//}

	//// 로그 출력
	//FString output = TEXT("TargetActors: ");
	//for (AActor* targetActor : TargetActors)
	//{
	//	output += targetActor->GetName() + TEXT("  /  ");
	//}
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s"), *output));
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
		if (targetActor)
			output += targetActor->GetName() + TEXT("  /  ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	PRINTLOG(TEXT("%s"), *output)
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

		// 제 1타겟으로 지정하고 있던 액터가 사라지면
		if (Actor == TargetActors[0])
		{
			TargetActors.RemoveAt(0);

			// 시야내에 적이 없으면
			if (TargetActors.IsEmpty())
			{	
				PRINTLOG(TEXT("Target actors is empty"));
				ClearFocus(EAIFocusPriority::Gameplay);
				MyBlackboard->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
				UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), Actor->GetActorLocation(), 15, 15, FRotator::ZeroRotator, FColor::Yellow, 10, 1);
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
			TargetActors.Remove(Actor);
		}
	}
}

void ASG_EnemyAIController::HandleAudioStimuls(AActor* Actor, FAIStimulus Stimulus)
{
	MyBlackboard->SetValueAsVector(TEXT("TargetLocation"), Stimulus.StimulusLocation);
	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), Stimulus.StimulusLocation, 15, 15, FRotator::ZeroRotator, FColor::Purple, 10, 1);

	PRINTLOG(TEXT("소리 들림"));
}
