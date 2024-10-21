// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ProjectileSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../SG_Enemy.h"
#include "Engine/CollisionProfile.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "HSB/MilitaryVRPawn.h"

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
	
	if (Owner->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, [this]()
			{
				if (this)
				{
					Owner->Destroy();
				}
			}, BulletLifeTime, false);
	}
}


// Called every frame
void USG_ProjectileSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	check(Owner); if (nullptr == Owner) return;
	if (!bBulletInitialized) return;

	StartLocation = Owner->GetActorLocation();
	FVector NextLocation = StartLocation + BulletVelocity * DeltaTime;
	_DeltaTime = DeltaTime;
	FHitResult OutHit;

	// TraceChannel
	TArray<AActor*> ActorsToIgnore;
	ETraceTypeQuery tracechannel = UEngineTypes::ConvertToTraceType(TraceChannel);
	ETraceTypeQuery bodychannel = UEngineTypes::ConvertToTraceType(BodyChannel);
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, tracechannel, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Red, FColor::Green, 1.5f);

	if (bHit)
	{
		ACharacter* hitCharacter = Cast<ACharacter>(OutHit.GetActor());

		// 캐릭터가 맞았을 때
		if (hitCharacter)
		{
			bool bBodyHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, bodychannel, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Purple, FColor::Green, 1.5f);
			if (bBodyHit)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Player or Enemy Hit!"));

				if (Owner->HasAuthority())
				{
					// 플레이어에게 데미지 처리
					AMilitaryVRPawn* Player = Cast<AMilitaryVRPawn>(hitCharacter);
					if (Player)
					{
						Player->DamageProcess(BulletDamage);
					}
					// 에너미에게 데미지 처리
					ASG_Enemy* Enemy = Cast<ASG_Enemy>(hitCharacter);
					if (Enemy)
					{
						Enemy->DamageProcess(BulletDamage);
					}
				}
				// 출혈 이펙트
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFXFactory, FTransform(FRotator(0), OutHit.ImpactPoint, FVector(.1)), true);

			}
			// 캐릭터의 캡슐 컴포넌트만 스쳐갔다면 그냥 지나가게 하고싶음
			else
			{	
				Owner->SetActorLocation(NextLocation);
				BulletVelocity = CalculateGravityAndDecelaration(BulletVelocity);
				return;
			}
		}
		// 데칼 소환
		FRotator DecalRotation = UKismetMathLibrary::MakeRotFromX(OutHit.ImpactNormal);
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalFactory, FVector(2.5), OutHit.ImpactPoint, DecalRotation, 100.0f);
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

