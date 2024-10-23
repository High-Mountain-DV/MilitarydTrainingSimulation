// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ProjectileSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../SG_Enemy.h"
#include "Engine/CollisionProfile.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"

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

	MyBullet = GetOwner();
	check(MyBullet); if (nullptr == MyBullet) return;

	BulletVelocity = MyBullet->GetActorForwardVector() * BulletSpeed;

	StartLocation = MyBullet->GetActorLocation();

	bBulletInitialized = true;
	
	if (MyBullet->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, [this]()
			{
				if (this)
				{
					MyBullet->Destroy();
				}
			}, BulletLifeTime, false);
	}
}


// Called every frame
void USG_ProjectileSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	check(MyBullet); if (nullptr == MyBullet) return;
	if (!bBulletInitialized) return;

	StartLocation = MyBullet->GetActorLocation();
	FVector NextLocation = StartLocation + BulletVelocity * DeltaTime;
	_DeltaTime = DeltaTime;
	FHitResult OutHit;

	// TraceChannel
	TArray<AActor*> ActorsToIgnore;
	//auto* ShooterActor = Cast<AActor>(MyBullet->GetInstigator());
	//check(ShooterActor); if (nullptr == ShooterActor) return;

	//ActorsToIgnore.Add(ShooterActor);

	ETraceTypeQuery tracechannel = UEngineTypes::ConvertToTraceType(TraceChannel);
	ETraceTypeQuery bodychannel = UEngineTypes::ConvertToTraceType(BodyChannel);
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, tracechannel, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Red, FColor::Green, 1.5f);

	if (bHit)
	{
		ACharacter* hitCharacter = Cast<ACharacter>(OutHit.GetActor());

		// 캐릭터가 맞았을 때
		if (hitCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("HitActor: %s"), *hitCharacter->GetName()));
			bool bBodyHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, bodychannel, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Purple, FColor::Green, 1.5f);
			if (bBodyHit)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Player or Enemy Hit!"));

				if (MyBullet->HasAuthority())
				{
					// 플레이어에게 데미지 처리
					// 
					// 에너미에게 데미지 처리
					ASG_Enemy* Enemy = Cast<ASG_Enemy>(hitCharacter);
					if (Enemy)
					{
						Enemy->DamageProcess(BulletDamage);
						UE_LOG(LogTemp, Warning, TEXT("OutHit.ImpactPoint: {%s}"), *OutHit.ImpactPoint.ToString());
					}
				}
				// 출혈 이펙트
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFXFactory, FTransform(FRotator(0), OutHit.ImpactPoint, FVector(.1)), true);

			}
			// 캐릭터의 캡슐 컴포넌트만 스쳐갔다면 그냥 지나가게 하고싶음
			else
			{	
				MyBullet->SetActorLocation(NextLocation);
				BulletVelocity = CalculateGravityAndDecelaration(BulletVelocity);
				return;
			}
		}
		// 데칼 소환
		FRotator DecalRotation = UKismetMathLibrary::MakeRotFromX(OutHit.ImpactNormal);
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalFactory, FVector(2.5), OutHit.ImpactPoint, DecalRotation, 100.0f);
		check(MyBullet); if (nullptr == MyBullet) return;

		GetWorld()->GetTimerManager().ClearTimer(DestroyHandle);
		MyBullet->Destroy();
	}
	else
	{
		check(MyBullet); if (nullptr == MyBullet) return;

		MyBullet->SetActorLocation(NextLocation);
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

