// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SG_WeaponMaster.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API ASG_WeaponMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASG_WeaponMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// -----------------------------Component----------------------------- //
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* Weapon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* Magazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UArrowComponent* FirePosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* MuzzlePosition;
	// -----------------------------Component----------------------------- //
	// 
	// -----------------------------BlueprintEditable----------------------------- //
	UPROPERTY(EditDefaultsOnly, Category = "Defaults|Bullet")
	TSubclassOf<AActor> BP_EnemyBullet;
		
	UPROPERTY(EditDefaultsOnly, Category = "Defaults|Bullet")
	int32 MaxBulletCount = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults|Bullet", BlueprintReadOnly)
	int32 BulletCount = MaxBulletCount;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults|VFX")
	class UParticleSystem* FireVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults|SFX")
	USoundAttenuation* FireAttenuationSetting;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float PitchMin = -2;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float PitchMax = 6;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float YawMin = -1;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float YawMax = 1;

    UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
    float RecoilRecoverySpeed = 2.0f;                 // 복귀 속도
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
    float RecoilRecoveryDelay = 0.1f;                // 복귀 시작 전 딜레이
    
    bool bIsRecovering = false;                       // 현재 복구 중인지
    FTimerHandle RecoveryTimerHandle;                 // 복구 타이머

	// -----------------------------BlueprintEditable----------------------------- //
	
	UFUNCTION(BlueprintCallable)
	void SetShooter();

	UPROPERTY()
	class APawn* Shooter;

	void Aim(const FVector TargetLocation);
	FRotator AimRotation;
	UFUNCTION(BlueprintCallable)
	bool Fire(bool& OutStopShooting);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_SpawnFireVFX();

	void Recoil();
	void StartRecoilRecovery();
	void RecoverRecoil(float DeltaTime);
	bool bRLerpFlag = false;


	void Reloading();

	void HideMagazine();
	void ShowMagazine();
	int32 StopShootingProb = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 StopShootingDelta = 2;

	FActorSpawnParameters BulletSpawnParams;

	UPROPERTY(EditDefaultsOnly, Category = "Default | SFX")
	class USoundWave* FireSFX;
	UPROPERTY(EditDefaultsOnly, Category = "Default | SFX")
	float FireSFX_StartTime;
private:
};
