// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SG_EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_EnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UPROPERTY()
	class ASG_Enemy* Me;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASG_EnemyMagazine> MagazineFactory;

	UPROPERTY()
	class ASG_EnemyMagazine* GrabMagazine;

	UPROPERTY()
	class AActor* NewGrabMagazine;

	// AnimNotify
	UFUNCTION()
	void AnimNotify_DettachMagazine();

	UFUNCTION()
	void AnimNotify_ThrowMagazine();

	UFUNCTION()
	void AnimNotify_GrabNewMagazine();

	UFUNCTION()
	void AnimNotify_CoupleMagazine();
};

