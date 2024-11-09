// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SG_EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ASG_EnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASG_EnemyAIController();

protected:
	FRotator SmoothTargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 30.0f;

public:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY()
	class ASG_Enemy* Me;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBehaviorTree* BT_Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UBlackboardComponent* MyBlackboard;

	UPROPERTY()
	class UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<class AActor*> TargetActors;
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
private:
	void HandleVisualStimuls(AActor* Actor, FAIStimulus Stimulus);
	void HandleAudioStimuls(AActor* Actor, FAIStimulus Stimulus);
};
