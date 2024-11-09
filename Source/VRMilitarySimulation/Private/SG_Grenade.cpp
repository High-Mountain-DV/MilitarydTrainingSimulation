// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Grenade.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HSB/PlayerVRCharacter.h"
#include "../SG_Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/ArrowComponent.h"

// Sets default values
ASG_Grenade::ASG_Grenade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	BaseMesh->SetUseCCD(true);
	BaseMesh->bTraceComplexOnMove = true;
	BaseMesh->SetCollisionObjectType(ECC_GameTraceChannel10);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempBaseMesh(TEXT("/Script/Engine.StaticMesh'/Game/MilitarySimulator/JSG/Assets/Grenede/mk2_Base.mk2_Base'"));
	if (tempBaseMesh.Succeeded())
	{
		BaseMesh->SetStaticMesh(tempBaseMesh.Object);
	}

	ExplosionRangeComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRangeComp"));
	ExplosionRangeComp->SetupAttachment(BaseMesh);
	ExplosionRangeComp->SetSphereRadius(ExplosionRangeRadius);
	ExplosionRangeComp->SetCollisionObjectType(ECC_GameTraceChannel6);

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	LeverMesh->SetupAttachment(BaseMesh);
	LeverMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempLeverMesh(TEXT("/Script/Engine.StaticMesh'/Game/MilitarySimulator/JSG/Assets/Grenede/mk2_Lever.mk2_Lever'"));
	if (tempLeverMesh.Succeeded())
	{
		LeverMesh->SetStaticMesh(tempLeverMesh.Object);
	}

	PinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PinMesh"));
	PinMesh->SetupAttachment(BaseMesh);
	PinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempPinMesh(TEXT("/Script/Engine.StaticMesh'/Game/MilitarySimulator/JSG/Assets/Grenede/mk2_Pin.mk2_Pin'"));
	if (tempPinMesh.Succeeded())
	{
		PinMesh->SetStaticMesh(tempPinMesh.Object);
	}

	RingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMesh"));
	RingMesh->SetupAttachment(BaseMesh);
	RingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempRingMesh(TEXT("/Script/Engine.StaticMesh'/Game/MilitarySimulator/JSG/Assets/Grenede/mk2_Ring.mk2_Ring'"));
	if (tempRingMesh.Succeeded())
	{
		RingMesh->SetStaticMesh(tempRingMesh.Object);
	}

	BaseMesh->SetLinearDamping(0.2);
	BaseMesh->SetGenerateOverlapEvents(false);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BaseMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	BaseMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	BaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	BaseMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	ExplosionRangeComp->SetGenerateOverlapEvents(false);
	ExplosionRangeComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionRangeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionRangeComp->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);
	ExplosionRangeComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	LeverMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PinMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RingMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeverMesh->SetGenerateOverlapEvents(false);
	PinMesh->SetGenerateOverlapEvents(false);
	RingMesh->SetGenerateOverlapEvents(false);
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempExplosionVFX(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempExplosionVFX.Succeeded())
	{
		ExplosionVFX = tempExplosionVFX.Object;
	}
}

// Called when the game starts or when spawned
void ASG_Grenade::BeginPlay()
{
	Super::BeginPlay();

	ExplosionRangeComp->OnComponentBeginOverlap.AddDynamic(this, &ASG_Grenade::OnExplosionRangeCompBeginOverlap);
	ExplosionRangeComp->OnComponentEndOverlap.AddDynamic(this, &ASG_Grenade::OnExplosionRangeCompEndOverlap);
}

// Called every frame
void ASG_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASG_Grenade::OnExplosionRangeCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("BeginOverlap")));
	ActorsInRange.Add(OtherActor);

	FString output = TEXT("Grenade ActorsInRange: ");
	for (int i = 0; i < ActorsInRange.Num(); i++)
	{
		output += FString::Printf(TEXT("%s / "), *ActorsInRange[i]->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, output);
}

void ASG_Grenade::OnExplosionRangeCompEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("EndOverlap")));
	ActorsInRange.Remove(OtherActor);

	FString output = TEXT("Grenade ActorsInRange: ");
	for (int i = 0; i < ActorsInRange.Num(); i++)
	{
		output += FString::Printf(TEXT("%s / "), *ActorsInRange[i]->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, output);

}

void ASG_Grenade::SetPhysicalOption()
{
	BaseMesh->SetSimulatePhysics(true);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASG_Grenade::Active(class ACharacter* GrenadeInstigator)
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("!!!Grenade Active!!!")));
		FTimerHandle ExplosionHandle, ExplosionRangeCollisionHandle;
		GetWorld()->GetTimerManager().SetTimer(ExplosionHandle, this, &ASG_Grenade::ExplodeGrenade, DelayTime, false);
		GetWorld()->GetTimerManager().SetTimer(ExplosionRangeCollisionHandle, [this]()
			{
				ExplosionRangeComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				ExplosionRangeComp->SetGenerateOverlapEvents(true);
				//UE_LOG(LogTemp, Warning, TEXT("Grenade ExplosionRangeComp 콜리전 활성화"));
			}, 2.0f, false);
	}
}

FVector ASG_Grenade::GetThrowVelocityToTarget(const FVector& StartLocation, const FVector& TargetLocation, const float TimeMultiplier /*= 0.7f*/)
{
	// 3. 전체 변위 계산 (높이 차이 포함)
	FVector Displacement = TargetLocation - StartLocation;
	float TotalDistance = Displacement.Size();

	// 4. 최소 거리 체크
	const float MinDistance = 10.0f;
	if (TotalDistance < MinDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target too close, distance: %f"), TotalDistance);
		return FVector::ZeroVector;
	}

	// 5. 수평 거리와 높이 차이 계산
	float HorizontalDistance = FVector(Displacement.X, Displacement.Y, 0.0f).Size();
	float HeightDifference = Displacement.Z;

	// 6. 물리 상수
	const float Gravity = -980.0f;  // 실제 중력 가속도 (cm/s^2)

	// 7. 시간 계산 개선
	float EstimatedTime;
	if (TimeMultiplier > 0)
	{
		// 거리에 따른 비선형 시간 계산
		EstimatedTime = FMath::Sqrt(2.0f * HorizontalDistance / FMath::Abs(Gravity)) * TimeMultiplier;
		EstimatedTime = FMath::Clamp(EstimatedTime, 0.1f, 10.0f);  // 합리적인 범위로 제한
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid TimeMultiplier"));
		return FVector::ZeroVector;
	}

	// 8. 속도 계산
	float VerticalVelocity;
	float HorizontalVelocity;

	// 수직 속도 계산 (높이 차이 고려)
	VerticalVelocity = (HeightDifference - (0.5f * Gravity * EstimatedTime * EstimatedTime)) / EstimatedTime;

	// 수평 속도 계산
	HorizontalVelocity = HorizontalDistance / EstimatedTime;


	// 9. 최종 속도 벡터 계산
	FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation);
	FVector HorizontalVelocityVector = FVector(
		DirectionUnitVector.X * HorizontalVelocity,
		DirectionUnitVector.Y * HorizontalVelocity,
		VerticalVelocity
	);

	// 10. 속도 제한
	const float MaxVelocity = 10000.0f;  // 최대 속도 제한 (cm/s)
	if (HorizontalVelocityVector.Size() > MaxVelocity)
	{
		UE_LOG(LogTemp, Warning, TEXT("Velocity exceeded maximum limit, clamping"));
		HorizontalVelocityVector = HorizontalVelocityVector.GetClampedToSize(0.0f, MaxVelocity);
	}

	// 11. 디버그 정보
	UE_LOG(LogTemp, Log, TEXT("Throw Calculation: Distance=%f, Time=%f, Velocity=%s"),
		TotalDistance, EstimatedTime, *HorizontalVelocityVector.ToString());

	HorizontalVelocityVector *= FMath::RandRange(0.8f, 0.95f);

	return HorizontalVelocityVector;
}

bool ASG_Grenade::CheckTrajectoryCollision(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& TargetLocation, const FVector& Velocity, AActor* ActorToIgnore /*= nullptr*/, bool bDebugBoxOn /*= false*/)
{
	// 가상의 물리 객체 생성
	FPredictProjectilePathParams PredictParams(
		3.0f,             // 발사체 반지름
		StartLocation,    // 시작 위치
		Velocity,         // 초기 속도
		3.0f,             // 최대 시뮬레이션 시간
		ECollisionChannel::ECC_GameTraceChannel9,  // 충돌 채널 (GrenedeTrajectory)
		ActorToIgnore     // 무시할 액터
	);

	PredictParams.bTraceComplex = false;
	if (bDebugBoxOn)
	{
		PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictParams.DrawDebugTime = 2.0f;
	}
	else
		PredictParams.DrawDebugType = EDrawDebugTrace::None;
	FPredictProjectilePathResult PredictResult;

	// 언리얼 엔진의 물리 기반 예측 사용
	bool bHit = UGameplayStatics::PredictProjectilePath(WorldContextObject, PredictParams, PredictResult);
	FString HitActorName;
	FString HitComponentName;
	if (PredictResult.HitResult.GetActor())
	{
		bool bGroundHit = PredictResult.HitResult.GetActor()->ActorHasTag(TEXT("Ground"));
		if (bGroundHit) return false;
		HitActorName = PredictResult.HitResult.GetActor()->GetName();
	}
	if (PredictResult.HitResult.GetComponent())
		HitComponentName = PredictResult.HitResult.GetComponent()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("HitActorName: %s, HitComponentName: %s"), *HitActorName, *HitComponentName);
	if (bHit)
	{
		float dist = FVector::Dist(PredictResult.HitResult.ImpactPoint, TargetLocation);
		if (dist >= 500.0f)
		{
			return true; // 장애물 있음
		}
	}

	return false;
}

void ASG_Grenade::Throw(const FVector& TargetLocation)
{
	FVector Velocity = GetThrowVelocityToTarget(GetActorLocation(), TargetLocation);
	BaseMesh->AddImpulse(Velocity);
}

void ASG_Grenade::ExplodeGrenade()
{
	MulticastRPC_SpawnEmitterAtLocation(ExplosionVFX, FTransform(FRotator::ZeroRotator, GetActorLocation(), FVector(10)), true);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 10.0f, this, 3000, TEXT("Grenade"));
	TArray<AActor*> DamagedActors;
	TArray<FVector> Directions;
	TArray<float> Dists;

	//UE_LOG(LogTemp, Warning, TEXT("ExplodeGrenade, TargetNums: %d"), ActorsInRange.Num());
	FString output = TEXT("ActorsInRange: ");
	for (int i = 0; i < ActorsInRange.Num(); i++)
	{
		output += FString::Printf(TEXT("%s / "), *ActorsInRange[i]->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	ETraceTypeQuery tracechannel = UEngineTypes::ConvertToTraceType(ExplosionTraceChannel);
	FVector start = GetActorLocation();
	TArray<AActor*>ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FVector ReferenceLocation = GetActorLocation(); // 현재 액터의 위치
	ActorsInRange.Sort(FActorDistanceSortPredicate(ReferenceLocation));
	for (AActor* actorInRange : ActorsInRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grenade processing: %s, dist: %f"), *actorInRange->GetName(), FVector::Dist(ReferenceLocation, actorInRange->GetActorLocation()));
		FHitResult outHit;
		FVector end = actorInRange->ActorHasTag(TEXT("Dummy")) ? actorInRange->GetComponentByClass<UArrowComponent>()->GetComponentLocation() : actorInRange->GetActorLocation();
		//end += (end - start);
		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, tracechannel, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, outHit, true, FColor::Red, FColor::Green, 20);
		if (bHit)
		{
			if (outHit.GetActor() == actorInRange)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is damaged by grenade"), *actorInRange->GetName());
				DamagedActors.Add(actorInRange);
				Directions.Add((end - start).GetSafeNormal());
				Dists.Add(FVector::Dist(start, end));
				ActorsToIgnore.Add(outHit.GetActor());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("벽으로 숨음"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("거리 밖으로 회피"));
		}
	}

	for (int i = 0; i < DamagedActors.Num(); i++)
	{
		if (DamagedActors[i])
		{
			if (DamagedActors[i]->ActorHasTag(TEXT("Dummy")))
			{
				FVector DummyForce = (Directions[i].GetSafeNormal() + FVector(0, 0, .5)) * 30000;
				DamagedActors[i]->GetComponentByClass<USkeletalMeshComponent>()->AddImpulse(Directions[i] * 10000);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("DamagedActor: %s"), *DamagedActors[i]->GetName());
				ApplyExplosionDamage(DamagedActors[i], Directions[i], Dists[i]);
			}
		}

		MulticastRPC_Destroy();
	}
}
void ASG_Grenade::ApplyExplosionDamage(AActor* HitActor, const FVector& Direction, float Dist)
{
	ACharacter* hitCharacter = Cast<ACharacter>(HitActor);
	check(hitCharacter); if (nullptr == hitCharacter) return;

	if (GetInstigator()->HasAuthority())
	{
		if (hitCharacter)
		{
			APlayerVRCharacter* Player = Cast<APlayerVRCharacter>(hitCharacter);
			if (Player)
			{
				Player->DamageProcess(100);
			}
			else
			{
				// 에너미에게 데미지 처리
				ASG_Enemy* Enemy = Cast<ASG_Enemy>(hitCharacter);
				if (Enemy)
				{
					Enemy->DamageProcess(100, TEXT(""), Direction, GetInstigator());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DummyEnemy Add Impulse"));
			TSubclassOf<USkeletalMeshComponent> SkeletalMesh;
			Cast<USkeletalMeshComponent>(HitActor->GetComponentByClass(SkeletalMesh))->AddImpulse(FVector(1000));
		}
	}
}

void ASG_Grenade::MulticastRPC_SpawnEmitterAtLocation_Implementation(UParticleSystem* ParticleToSpawn, const FTransform& SpawnTransform, bool bAutoDestroy /*= true*/)
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastRPC_SpawnEmitterAtLocation_Implementation, %s"), *ParticleToSpawn->GetName());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleToSpawn, SpawnTransform, bAutoDestroy);
}

void ASG_Grenade::MulticastRPC_Destroy_Implementation()
{
	Destroy();
}


