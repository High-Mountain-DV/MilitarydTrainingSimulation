// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SG_Grenede.generated.h"

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
class VRMILITARYSIMULATION_API ASG_Grenede : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASG_Grenede();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly)
	class UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* ExplosionRangeComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* GrenedeMeshComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LeverMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* PinMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* RingMesh;

	//UPROPERTY(EditDefaultsOnly)
	//class UStaticMeshComponent* FragmentMeshComp;

	//// 파편/쇠구슬 메시 배열
	//UPROPERTY(EditDefaultsOnly)
 //   TArray<UStaticMeshComponent*> FragmentMeshes;

	//UPROPERTY(EditDefaultsOnly)
	//int32 FragmentNums = 20;

	UPROPERTY(EditDefaultsOnly)
	float DelayTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel6;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	class UParticleSystem* ExplosionVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Default|Throw")
	float TimeMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Default|Throw")
	float MinNoise = -2;

	UPROPERTY(EditDefaultsOnly, Category = "Default|Throw")
	float MaxNoise = 2;
	UPROPERTY()
	TArray<AActor* > ActorsInRange;

	UFUNCTION()
	void OnExplosionRangeCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExplosionRangeCompEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetPhysicalOption();

	UFUNCTION(BlueprintCallable)
	void Active(class ACharacter* GrenedeInstigator);
	bool CheckTrajectoryCollision(const FVector& TargetLocation, const FVector& Velocity);
	bool ThrowWithCheck(const FVector& TargetLocation);
	void Throw(const FVector& TargetLocation);
private:
	FVector GetThrowVelocityToTarget(const FVector& TargetLocation);
	void ExplodeGrenede();
	void ApplyExplosionDamage(AActor* HitActor, const FVector& Direction, float Dist);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SpawnEmitterAtLocation(UParticleSystem* ParticleToSpawn, const FTransform& SpawnTransform, bool bAutoDestroy = true);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Destroy();
};
