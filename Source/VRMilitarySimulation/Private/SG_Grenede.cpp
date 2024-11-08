// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Grenede.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HSB/PlayerVRCharacter.h"
#include "../SG_Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
const float ExplosionRange = 600;
const float GrenedeSize = 2;
ASG_Grenede::ASG_Grenede()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->SetupAttachment(RootComponent);
	CapsuleComp->SetCapsuleHalfHeight(12.5);
	CapsuleComp->SetCapsuleRadius(6);

	ExplosionRangeComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRangeComp"));
	ExplosionRangeComp->SetupAttachment(CapsuleComp);
	ExplosionRangeComp->SetSphereRadius(ExplosionRange);

	GrenedeMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenedeMeshComp"));
	GrenedeMeshComp->SetupAttachment(CapsuleComp);
	GrenedeMeshComp->SetRelativeScale3D(FVector(GrenedeSize));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(GrenedeMeshComp);
	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	LeverMesh->SetupAttachment(GrenedeMeshComp);
	PinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PinMesh"));
	PinMesh->SetupAttachment(GrenedeMeshComp);
	RingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMesh"));
	RingMesh->SetupAttachment(GrenedeMeshComp);

	//FragmentMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FragmentMeshComp"));
	//FragmentMeshComp->SetupAttachment(CapsuleComp);
	//ConstructorHelpers::FObjectFinder<UStaticMesh> tempCubeMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	//if (tempCubeMesh.Succeeded())
	//{
	//	for (int i = 0; i < FragmentNums; i++)
	//	{
	//		UStaticMeshComponent* NewFragment = CreateDefaultSubobject<UStaticMeshComponent>(FName(*FString::Printf(TEXT("Fragment_%d"), i)));
	//		// 컴포넌트 등록 및 초기화
	//		NewFragment->SetupAttachment(FragmentMeshComp);
	//		NewFragment->SetStaticMesh(tempCubeMesh.Object);
	//		NewFragment->SetRelativeScale3D(FVector(0.008));
	//		// 랜덤한 위치에 배치 (수류탄 내부)
	//		FVector RandomOffset = FMath::VRand() * 3.6f; // 반경 내 랜덤 위치
	//		NewFragment->SetRelativeLocation(RandomOffset);

	//		// 배열에 추가
	//		FragmentMeshes.Add(NewFragment);
	//	}
	//}
}

// Called when the game starts or when spawned
void ASG_Grenede::BeginPlay()
{
	Super::BeginPlay();

	ExplosionRangeComp->OnComponentBeginOverlap.AddDynamic(this, &ASG_Grenede::OnExplosionRangeCompBeginOverlap);
	ExplosionRangeComp->OnComponentEndOverlap.AddDynamic(this, &ASG_Grenede::OnExplosionRangeCompEndOverlap);
}

// Called every frame
void ASG_Grenede::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASG_Grenede::OnExplosionRangeCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("BeginOverlap")));
	ActorsInRange.Add(OtherActor);

	FString output = TEXT("Grenede ActorsInRange: ");
	for (int i = 0; i < ActorsInRange.Num(); i++)
	{
		output += FString::Printf(TEXT("%s / "), *ActorsInRange[i]->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, output);
}

void ASG_Grenede::OnExplosionRangeCompEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("EndOverlap")));
	ActorsInRange.Remove(OtherActor);

	FString output = TEXT("Grenede ActorsInRange: ");
	for (int i = 0; i < ActorsInRange.Num(); i++)
	{
		output += FString::Printf(TEXT("%s / "), *ActorsInRange[i]->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, output);
}

void ASG_Grenede::SetPhysicalOption()
{
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASG_Grenede::Active(class ACharacter* GrenedeInstigator)
{
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("!!!Grenede Active!!!")));
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ASG_Grenede::ExplodeGrenede, DelayTime, false);
	}
}

FVector ASG_Grenede::GetThrowVelocityToTarget(const FVector& TargetLocation)
{
	FVector ret;

	FVector Sub = TargetLocation - GetActorLocation();
	Sub.Z = 0;

	FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), TargetLocation);

	// Horizontal Distance
	float S = Sub.Length();
	// Time
	float t = (S / 1000) * TimeMultiplier;
	// Gravity
	float g = -490;
	// Vertical Component of velocity
	float v = -t * g * 0.5;

	// Horizontal Component of velocity
	float u = (S - (0.5 * g * FMath::Square(t))) / t;

	ret.X = DirectionUnitVector.X * u;
	ret.Y = DirectionUnitVector.Y * u;
	ret.Z = v;


	//ret += FVector(FMath::RandRange(MinNoise, MaxNoise), FMath::RandRange(MinNoise, MaxNoise), FMath::RandRange(MinNoise, MaxNoise));
	//ret*= FMath::RandRange(0.8, 1.0);
	return ret;
}

bool ASG_Grenede::CheckTrajectoryCollision(const FVector& TargetLocation, const FVector& Velocity)
{
	FVector StartPos = GetActorLocation();
	const float TimeStep = 0.1f;  // 0.1초 간격으로 체크
	const float MaxTime = (TargetLocation - StartPos).Size() / 700.0f;  // 예상 도착 시간

	for (float Time = 0; Time <= MaxTime; Time += TimeStep)
	{
		// 포물선 상의 점 계산
		FVector Position = StartPos + Velocity * Time;
		Position.Z += -490.0f * Time * Time * 0.5f;  // 중력 적용

		// 장애물 체크
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);  // 자기 자신은 무시

		// 이전 위치에서 현재 위치까지 LineTrace
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartPos,
			Position,
			ECC_Visibility,
			QueryParams
		);

		if (bHit)
		{
			// 장애물이 있다면 true 반환
			return true;
		}

		StartPos = Position;  // 다음 체크를 위해 시작점 업데이트
	}

	return false;  // 장애물이 없으면 false 반환
}


bool ASG_Grenede::ThrowWithCheck(const FVector& TargetLocation)
{
	FVector Velocity = GetThrowVelocityToTarget(TargetLocation);

	if (CheckTrajectoryCollision(TargetLocation, Velocity))
	{
		UE_LOG(LogTemp, Warning, TEXT("궤적상에 장애물로 수류탄 안던짐"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("ThrowVelocity: %s"), *Velocity.ToString());
	CapsuleComp->AddImpulse(Velocity);
	return true;
}

void ASG_Grenede::Throw(const FVector& TargetLocation)
{
	FVector Velocity = GetThrowVelocityToTarget(TargetLocation);
	CapsuleComp->AddImpulse(Velocity);
}

void ASG_Grenede::ExplodeGrenede()
{
	MulticastRPC_SpawnEmitterAtLocation(ExplosionVFX, FTransform(FRotator::ZeroRotator, GetActorLocation(), FVector(10)), true);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 10.0f, this, 3000, TEXT("Grenede"));
	TArray<AActor*> DamagedActors;
	TArray<FVector> Directions;
	TArray<float> Dists;

	//UE_LOG(LogTemp, Warning, TEXT("ExplodeGrenede, TargetNums: %d"), ActorsInRange.Num());
	FString output = TEXT("ActorsInRange: ");
	for (int i = 0; i < ActorsInRange.Num(); i++)
	{
		output += FString::Printf(TEXT("%s / "), *ActorsInRange[i]->GetName());
	}
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	ETraceTypeQuery tracechannel = UEngineTypes::ConvertToTraceType(TraceChannel);
	FVector start = GetActorLocation();
	TArray<AActor*>ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FVector ReferenceLocation = GetActorLocation(); // 현재 액터의 위치
	ActorsInRange.Sort(FActorDistanceSortPredicate(ReferenceLocation));
	for (AActor* actorInRange : ActorsInRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grenede processing: %s, dist: %f"), *actorInRange->GetName(), FVector::Dist(ReferenceLocation, actorInRange->GetActorLocation()));
		FHitResult outHit;
		FVector end = actorInRange->GetActorLocation();
		//end += (end - start);
		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, tracechannel, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, outHit, true, FColor::Red, FColor::Green, 20);
		if (bHit)
		{
			if (outHit.GetActor() == actorInRange)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is damaged by grenede"), *actorInRange->GetName());
				DamagedActors.Add(actorInRange);
				Directions.Add((end - start).GetSafeNormal());
				Dists.Add(FVector::Dist(start, end));
				ActorsToIgnore.Add(outHit.GetActor());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("거리 밖으로 회피"));
		}
	}

	for (int i = 0; i < DamagedActors.Num(); i++)
	{
		ApplyExplosionDamage(DamagedActors[i], Directions[i], Dists[i]);
	}

	MulticastRPC_Destroy();
}

void ASG_Grenede::ApplyExplosionDamage(AActor* HitActor, const FVector& Direction, float Dist)
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

void ASG_Grenede::MulticastRPC_SpawnEmitterAtLocation_Implementation(UParticleSystem* ParticleToSpawn, const FTransform& SpawnTransform, bool bAutoDestroy /*= true*/)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleToSpawn, SpawnTransform, bAutoDestroy);
}

void ASG_Grenede::MulticastRPC_Destroy_Implementation()
{
	Destroy();
}


