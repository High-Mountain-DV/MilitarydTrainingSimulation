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

	void SetWeapon();

	UFUNCTION()
	void ServerRPC_PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	bool Fire(bool& OutStopShooting);
	void Aim(const FVector TargetLocation);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UChildActorComponent* WeaponComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UArrowComponent* DebugArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class ASG_WeaponMaster> WeaponClass;

	UPROPERTY(EditDefaultsOnly,	BlueprintReadOnly, Category = "Default|State")
	float MaxHP = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	class ASG_WeaponMaster* CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon();

	UPROPERTY()
	class USG_EnemyAnimInstance* Anim;

	UFUNCTION()
	void OnRep_HP();

	__declspec(property(get = GetHP, put = SetHP)) float HP;

	UFUNCTION(BlueprintCallable)
	float GetHP();
	UFUNCTION(BlueprintCallable)
	void SetHP(float Value);
	UFUNCTION(BlueprintCallable)
	void DamageProcess(float Damage, const FName& BoneName, const FVector& ShotDirection, AActor* Shooter);

	UPROPERTY(EditDefaultsOnly, Category = "Default|Damage")
	float HeadShotMultiplier = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Default|Damage")
	float BodyShotMultiplier = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Default|Damage")
	float ArmOrLegShotMultiplier = 4;

	UFUNCTION(BlueprintCallable)
	void Reloading();

	int32 PointIndex = 1;

	UPROPERTY(BlueprintReadOnly)
	FVector LeftHandPos;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float AimPitch;
	UPROPERTY(BlueprintReadOnly, Replicated)
	float AimYaw;

	void SetAimOffsetAlpha(float AimOffsetAlpha);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetAimOffsetAlpha(float AimOffsetAlpha);

	UFUNCTION(BlueprintCallable)
	bool FindPathPoints(const FVector& TargetLocation, float Radius);
	TArray<FVector> PathPoints;
	FVector NextTargetLocation;
	float Speed;
	bool StartMovement;
	UPROPERTY(EditDefaultsOnly, Category = "Default | Debug")
	bool PathFindDebug = true;

	float AcceptableRadius = 50;

	UFUNCTION(BlueprintCallable)
	void DebugPoints(const TArray<FVector>& Array);

	FVector GetDirectionToTarget();
	FVector DirectionVector;

	bool ArriveAtLocation(FVector Location);
	void StopMovement();

	void HideWeaponMagazine();
	void ShowWeaponMagazine();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyImpactToBone(const FName& BoneName, const FVector& ShotDirection);

	UPROPERTY(EditDefaultsOnly, Category = "Default|Recoil")
	float RecoilPitchMinOffset = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Default|Recoil")
	float RecoilPitchMaxOffset = 8;
	UPROPERTY(EditDefaultsOnly, Category = "Default|Recoil")
	float RecoilYawMinOffset= -2;
	UPROPERTY(EditDefaultsOnly, Category = "Default|Recoil")
	float RecoilYawMaxOffset = -2;

	UPROPERTY(EditDefaultsOnly, Category = "Default|Factory")
	TSubclassOf<class ASG_Grenede> BP_Grenede;
	UPROPERTY(EditDefaultsOnly, Category = "Default|Factory")
	TSubclassOf<class ASG_DummyEnemy> BP_DummyEnemy;

private:

	UPROPERTY(ReplicatedUsing = OnRep_HP)
	float hp = MaxHP;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bDead;
	void AI_Move_To(float DeltaTime);

	UPROPERTY()
	class ASG_Grenede* Grenede;

	UPROPERTY(Replicated)
	float DestinationAimPitch;
	UPROPERTY(Replicated)
	float DestinationAimYaw;
	UPROPERTY(Replicated)
	bool bAiming;

	void LerpAimoffset(float DeltaTime);
	void DieProcess(const FName& BoneName, const FVector& ShotDirection, AActor* Shooter);

	UFUNCTION(Server, Reliable)
	void ServerRPC_DieProcess(const FName& BoneName, const FVector&ShotDirection, AActor* Shooter);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SpawnDummyEnemy(const FTransform& SpawnTransform, const FVector& ShotDirection);
	void Recoil();
};
