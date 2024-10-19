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

	bool Fire();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UArrowComponent* FirePosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* Muzzle;

	float MaxHP = 100;
	UPROPERTY(EditDefaultsOnly,	BlueprintReadOnly)
	int32 MaxBulletCount = 40;

	UFUNCTION()
	void OnRep_HP();

	__declspec(property(get = GetHP, put = SetHP)) float HP;

	UFUNCTION(BlueprintCallable)
	float GetHP();
	UFUNCTION(BlueprintCallable)
	void SetHP(float Value);
	UFUNCTION(BlueprintCallable)
	void DamageProcess(float Damage);

	UPROPERTY(BlueprintReadWrite)
	int32 BulletCount;

	UFUNCTION(BlueprintCallable)
	void Reloading();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> BP_EnemyBullet;
	
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* FireVFX;
private:

	UPROPERTY(ReplicatedUsing = OnRep_HP)
	float hp = MaxHP;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bDead;

	
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_SpawnFireVFX(const FTransform& SpawnTransform);
};
