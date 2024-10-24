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
	bool Fire(bool& OutStopShooting);
	void Aim(const FVector TargetLocation);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UChildActorComponent* WeaponComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UArrowComponent* DebugArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class ASG_WeaponMaster> WeaponClass;

	float MaxHP = 100;
	UPROPERTY(EditDefaultsOnly,	BlueprintReadOnly)
	int32 MaxBulletCount = 40;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ASG_WeaponMaster* CurrentWeapon;

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
	void DamageProcess(float Damage, const FString& BoneName, const FVector& ShotDirection, AActor* Shooter);

	UPROPERTY(EditDefaultsOnly)
	float HeadShotMultiplier = 10;

	UPROPERTY(EditDefaultsOnly)
	float BodyShotMultiplier = 5;

	UPROPERTY(EditDefaultsOnly)
	float ArmOrLegShotMultiplier = 4;


	UPROPERTY(BlueprintReadWrite)
	int32 BulletCount;

	UFUNCTION(BlueprintCallable)
	void Reloading();

	int32 PointIndex = 1;

	UPROPERTY(BlueprintReadOnly)
	FVector LeftHandPos;

	UPROPERTY(BlueprintReadOnly)
	float AimPitch;
	UPROPERTY(BlueprintReadOnly)
	float AimYaw;

	UFUNCTION(BlueprintCallable)
	bool FindPathPoints(const FVector& TargetLocation, float Radius);
	TArray<FVector> PathPoints;
	FVector NextTargetLocation;
	float Speed;
	bool StartMovement;
	UPROPERTY(EditDefaultsOnly)
	bool PathFindDebug = true;

	float AcceptableRadius = 50;

	UFUNCTION(BlueprintCallable)
	void DebugPoints(const TArray<FVector>& Array);

	FVector GetDirectionToTarget();
	FVector DirectionVector;

	bool ArriveAtLocation(FVector Location);
	void StopMovement();

	UPROPERTY(EditDefaultsOnly)
	float AimOffsetYawCoef= 1.05f;
	UPROPERTY(EditDefaultsOnly)
	float AimOffsetPitchCoef = 1;

private:

	UPROPERTY(ReplicatedUsing = OnRep_HP)
	float hp = MaxHP;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bDead;
	void AI_Move_To(float DeltaTime);
public:
private:
	float DestinationAimPitch;
	float DestinationAimYaw;
	bool bAiming;

	void LerpAimoffset(float DeltaTime);
};
