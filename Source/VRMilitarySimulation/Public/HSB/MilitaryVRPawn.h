// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MilitaryVRPawn.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API AMilitaryVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMilitaryVRPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 데미지 받는 함수
	UFUNCTION(BlueprintCallable)
	void DamageProcess(float Damage);
	
	UPROPERTY(BlueprintReadWrite)
	float Max_hp = 40;
	UPROPERTY(BlueprintReadWrite)
	float p_hp = Max_hp;

private:
	
	
};
