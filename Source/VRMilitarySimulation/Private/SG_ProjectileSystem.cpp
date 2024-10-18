// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ProjectileSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
USG_ProjectileSystem::USG_ProjectileSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USG_ProjectileSystem::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	check(Owner); if (nullptr == Owner) return;

	BulletVelocity = Owner->GetActorForwardVector() * BulletSpeed;

	StartLocation = Owner->GetActorLocation();

	bBulletInitialized = true;

	
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, [this]()
		{
			if (this)
			{
				Owner->Destroy();
			}
		}, BulletLifeTime, false);
}


// Called every frame
void USG_ProjectileSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bBulletInitialized) return;
	check(Owner); if (nullptr == Owner) return;

	StartLocation = Owner->GetActorLocation();
	FVector NextLocation = StartLocation + BulletVelocity * DeltaTime;
	_DeltaTime = DeltaTime;
	FHitResult OutHit;

	// TraceChannel
	TArray<AActor*> ActorsToIgnore;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, TraceChannel, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	if (bHit)
	{
		ACharacter* Player = Cast<ACharacter>(OutHit.GetActor());

		// 플레이어가 맞았을 때
		if (Player)
		{
			bool bBodyHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, BodyChannel, true, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Purple);
			if (bBodyHit)
			{
				// 적에게 데미지 처리

				// 출혈 이펙트
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFXFactory, FTransform(FRotator(0), OutHit.ImpactPoint, FVector(.1)), true);
			}
			
		}
		// 데칼 소환
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalFactory, FVector(2.5), OutHit.ImpactPoint, FRotator(0), 100.0f);
		check(Owner); if (nullptr == Owner) return;

		GetWorld()->GetTimerManager().ClearTimer(DestroyHandle);
		Owner->Destroy();
	}
	else
	{
		check(Owner); if (nullptr == Owner) return;

		Owner->SetActorLocation(NextLocation);
		BulletVelocity = CalculateGravityAndDecelaration(BulletVelocity);
	}
}

FVector USG_ProjectileSystem::CalculateGravityAndDecelaration(FVector Velocity) const
{
	return Velocity + FVector(0, 0, Gravity * 100 * BulletMass * -1 + DragForce()) * _DeltaTime;
}

float USG_ProjectileSystem::DragForce() const
{
	return -(AirDensity * FMath::Square(BulletSpeed) * CrossSection * DragCoefficiency ) / 2;
}

