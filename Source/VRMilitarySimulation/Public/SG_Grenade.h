// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SG_Grenade.generated.h"

class FActorDistanceSortPredicate
{
public:
	FActorDistanceSortPredicate(const FVector& InReferenceLocation)
		: ReferenceLocation(InReferenceLocation)
	{}

	bool operator()(const AActor& A, const AActor& B) const
	{
		// 배우터 위치와의 거리 계산
		float DistToA = (A.GetActorLocation() - ReferenceLocation).Size();
		float DistToB = (B.GetActorLocation() - ReferenceLocation).Size();

		// 거리 기준 오름차순 정렬
		return DistToA < DistToB;
	}

private:
	FVector ReferenceLocation;
};

UCLASS()
class VRMILITARYSIMULATION_API ASG_Grenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASG_Grenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static bool CheckTrajectoryCollision(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& TargetLocation, const FVector& Velocity, class AActor* ActorToIgnore = nullptr, bool bDebugBoxOn = false);
	static FVector GetThrowVelocityToTarget(const FVector& StartLocation, const FVector& TargetLocation, const float TimeMultiplier = 0.7f);

	// Component
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* ExplosionRangeComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LeverMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* PinMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* RingMesh;

	// Custom Variables
	UPROPERTY(EditDefaultsOnly, Category = "----------------------------------------------Custom----------------------------------------------")
	float DelayTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "----------------------------------------------Custom----------------------------------------------")
	TEnumAsByte<ECollisionChannel> ExplosionTraceChannel = ECollisionChannel::ECC_GameTraceChannel6;

	UPROPERTY(EditDefaultsOnly, Category = "----------------------------------------------Custom----------------------------------------------")
	class UParticleSystem* ExplosionVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "----------------------------------------------Custom----------------------------------------------")
	class USoundBase* ExplosionSFX;

	UPROPERTY(EditDefaultsOnly, Category = "----------------------------------------------Custom----------------------------------------------")
	float ExplosionRangeRadius = 600;

	// Inner Variables
	UPROPERTY()
	TArray<AActor* > ActorsInRange;

	TArray<FString> EncounterPlayerLabels;
	TArray<FString> DamagedPlayerLabels;

	void Active(class ACharacter* GrenadeInstigator);
	
private:
	void Explode();
	void ApplyExplosionDamage(AActor* HitActor, const FVector& Direction, float Dist);
	void SetCollisionExplosionRangeComp();
		
	UFUNCTION()
	void OnExplosionRangeCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExplosionRangeCompEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
