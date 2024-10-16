// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SG_Enemy.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API ASG_Enemy : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	ASG_Enemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float MaxHP = 100;

	UFUNCTION()
	void OnRep_HP();

	__declspec(property(get = GetHP, put = SetHP)) float HP;

	float GetHP();
	void SetHP(float Value);

private:

	UPROPERTY(ReplicatedUsing = OnRep_HP)
	float hp = MaxHP;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bDead;
};
