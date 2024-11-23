// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SG_ProjectileSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRMILITARYSIMULATION_API USG_ProjectileSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USG_ProjectileSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* BulletFlybySound;

	UPROPERTY(EditDefaultsOnly)
	float FlybyDistance = 500.0f;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SpawnEmitterAtLocation(UParticleSystem* ParticleToSpawn, const FTransform& SpawnTransform, bool bAutoDestroy = true);
	void MulticastRPC_SpawnDecalAtLocation(UMaterialInterface* DecalToSpawn, const FVector& DecalSize = FVector(1), const FVector& Location = FVector(0), const FRotator& Rotation = FRotator(-90, 0, 0), float LifeSpan = 0);
	FVector CalculateGravityAndDecelaration(FVector Velocity) const;

	void PlayBulletFlyBySound();

	UFUNCTION(BlueprintPure)
	float DragForce() const;
	UPROPERTY()
	class AActor* MyBullet;
	UPROPERTY()
	class ASG_WeaponMaster* Weapon;
	UPROPERTY()
	class APawn* Shooter;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Camera;
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	TEnumAsByte<ECollisionChannel> BodyChannel = ECollisionChannel::ECC_GameTraceChannel7;

	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BulletDamage = 10;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BulletSpeed = 37000;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BulletLifeTime = 5;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BulletMass = 0.015f;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float CrossSection = 1.04f;

	float AirDensity = 0.001225f;
	float DragCoefficiency = 0.012;
	float Gravity = 9.8;

	float _DeltaTime;

	FVector BulletVelocity;
	FVector StartLocation;
	bool bBulletInitialized;

	UPROPERTY(EditDefaultsOnly, Category = VFX)
	class UParticleSystem* BloodVFXFactory;

	UPROPERTY(EditDefaultsOnly, Category = VFX)
	class UMaterialInterface* BulletHoleDecalFactory;

	FTimerHandle DestroyHandle;

	ETraceTypeQuery tracechannel;
	ETraceTypeQuery bodychannel;
	TArray<AActor* >FoundPlayers;
	TArray<bool> bHasPlayedSounds;
};
