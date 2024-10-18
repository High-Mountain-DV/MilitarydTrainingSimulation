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
private:
	FVector CalculateGravityAndDecelaration(FVector Velocity) const;
	UFUNCTION(BlueprintPure)
	float DragForce() const;
	UPROPERTY()
	class AActor* Owner;
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery2;
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	TEnumAsByte<ETraceTypeQuery> BodyChannel = ETraceTypeQuery::TraceTypeQuery17;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BulletSpeed = 37000;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BulletLifeTime = 10;

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
};
