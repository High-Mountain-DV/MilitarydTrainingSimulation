// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "CustomMotionControllerComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UCustomMotionControllerComponent : public UMotionControllerComponent
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SetPlayerIndex(int32 NewPlayerIndex);

    UFUNCTION()
    void OnRep_PlayerIndex();

    // Lifetime Replication 설정 함수 (Replicated 변수를 자동으로 복제)
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
