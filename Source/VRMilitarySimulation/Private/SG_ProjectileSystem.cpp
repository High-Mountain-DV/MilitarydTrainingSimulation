// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ProjectileSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../SG_Enemy.h"
#include "Engine/CollisionProfile.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "SG_DummyEnemy.h"
#include "HSB/PlayerVRCharacter.h"
#include "../VRMilitarySimulation.h"

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
	
	auto* ShooterActor = Cast<AActor>(MyBullet->GetInstigator());
	check(ShooterActor); if (nullptr == ShooterActor) return;
	Shooter = Cast<APawn>(ShooterActor);
	ActorsToIgnore.Add(ShooterActor);

	tracechannel = UEngineTypes::ConvertToTraceType(TraceChannel);
	bodychannel = UEngineTypes::ConvertToTraceType(BodyChannel);

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

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACharacter::StaticClass(), TEXT("Player"), FoundPlayers);
	bHasPlayedSounds.SetNum(FoundPlayers.Num());
	for (bool& bSoundPlayed : bHasPlayedSounds)
	{
		bSoundPlayed = false;
	}
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerVRCharacter::StaticClass(), FoundPlayers);
}


// Called every frame
void USG_ProjectileSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(MyBullet); if (nullptr == MyBullet) return;
	if (!bBulletInitialized) return;

	PlayBulletFlyBySound();

	StartLocation = MyBullet->GetActorLocation();
	FVector NextLocation = StartLocation + BulletVelocity * DeltaTime;
	_DeltaTime = DeltaTime;
	FHitResult OutHit;
	if (nullptr == Shooter)
	{
		UE_LOG(LogTemp, Warning, TEXT("nullptr == Shooter"));
		return;
	}
	if (nullptr != Shooter)
	{
		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, tracechannel, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Red, FColor::Green, 1.5f);
		if (bHit)
		{
			ACharacter* hitCharacter = Cast<ACharacter>(OutHit.GetActor());
			ASG_DummyEnemy* dummyEnemy = Cast<ASG_DummyEnemy>(OutHit.GetActor());

			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor Name: {%s}"), *OutHit.GetActor()->GetName());
			// 캐릭터가 맞았을 때
			if (Shooter->HasAuthority())
			{
				if (hitCharacter)
				{
					bool bBodyHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, NextLocation, bodychannel, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Purple, FColor::Green, 1.5f);
					if (bBodyHit)
					{
						UE_LOG(LogTemp, Warning, TEXT("BoneName: {%s}"), *OutHit.BoneName.ToString());
						if (MyBullet->HasAuthority())
						{
							// 플레이어에게 데미지 처리
							APlayerVRCharacter* Player = Cast<APlayerVRCharacter>(hitCharacter);
							if (Player)
							{
								Player->DamageProcess(BulletDamage);
							}
							else
							{
								// 에너미에게 데미지 처리
								ASG_Enemy* Enemy = Cast<ASG_Enemy>(hitCharacter);
								if (Enemy)
								{
									Enemy->DamageProcess(BulletDamage, OutHit.BoneName, BulletVelocity.GetSafeNormal(), Shooter);
									UE_LOG(LogTemp, Warning, TEXT("OutHit.ImpactPoint: {%s}"), *OutHit.ImpactPoint.ToString());
								}

							}
						}
						// 출혈 이펙트
						MulticastRPC_SpawnEmitterAtLocation(BloodVFXFactory, FTransform(FRotator(0), OutHit.ImpactPoint, FVector(.1)), true);

					}
					// 캐릭터의 캡슐 컴포넌트만 스쳐갔다면 그냥 지나가게 하고싶음
					else
					{
						check(MyBullet); if (nullptr == MyBullet) return;
						MyBullet->SetActorLocation(NextLocation);
						BulletVelocity = CalculateGravityAndDecelaration(BulletVelocity);
						return;
					}
				}
				else if (dummyEnemy)
				{
					UE_LOG(LogTemp, Warning, TEXT("DummyEnemy Add Impulse"));
					dummyEnemy->Mesh->AddImpulse(BulletVelocity / 5);
				}
			}
			// 데칼 소환
			FRotator DecalRotation = UKismetMathLibrary::MakeRotFromX(OutHit.ImpactNormal);
			MulticastRPC_SpawnDecalAtLocation(BulletHoleDecalFactory, FVector(2.5), OutHit.ImpactPoint, DecalRotation, 100.0f);
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
	else
	{
		check(MyBullet); if (nullptr == MyBullet) return;

		MyBullet->SetActorLocation(NextLocation);
		BulletVelocity = CalculateGravityAndDecelaration(BulletVelocity);
	}
}

void USG_ProjectileSystem::MulticastRPC_SpawnEmitterAtLocation_Implementation(UParticleSystem* ParticleToSpawn, const FTransform& SpawnTransform, bool bAutoDestroy /*= true*/)
{
	UGameplayStatics::SpawnEmitterAtLocation(Shooter->GetWorld(), ParticleToSpawn, SpawnTransform, bAutoDestroy);
}

void USG_ProjectileSystem::MulticastRPC_SpawnDecalAtLocation(UMaterialInterface* DecalToSpawn, const FVector& DecalSize /*= FVector(1)*/, const FVector& Location /*= FVector(0)*/, const FRotator& Rotation /*= FRotator(-90, 0, 0)*/, float LifeSpan /*= 0*/)
{
	UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalFactory, DecalSize, Location, Rotation, LifeSpan);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), Location, FVector(5), FColor::Purple, FRotator::ZeroRotator, 10);

}

FVector USG_ProjectileSystem::CalculateGravityAndDecelaration(FVector Velocity) const
{
	return Velocity + FVector(0, 0, Gravity * 100 * BulletMass * -1 + DragForce()) * _DeltaTime;
}

void USG_ProjectileSystem::PlayBulletFlyBySound()
{
	FVector BulleLocation = MyBullet->GetActorLocation();

	// 각 플레이어와의 거리 체크
	for(int i = 0;i < FoundPlayers.Num(); i++)
	//for (AActor* Player : FoundPlayers)
	{
		AActor* Player = FoundPlayers[i];
		bool& bPlayedSound = bHasPlayedSounds[i];
		//PRINTLOG(TEXT("Player: %s"), *Player->GetName())

		if (IsValid(Player) && !bPlayedSound/* && Shooter != Player*/)
		{
			PRINTLOG(TEXT("		if (IsValid(Player) && !bPlayedSound && Shooter != Player)"))
			FVector PlayerLocation = Player->GetActorLocation();
			FVector BulletLocaiton = MyBullet->GetActorLocation();
			float DistanceToPlayer = FVector::Dist(PlayerLocation, BulletLocaiton);

			if (BulletFlybySound && DistanceToPlayer <= FlybyDistance)
			{
				PRINTLOG(TEXT("BulletFly Sound on"))
				UGameplayStatics::PlaySoundAtLocation(
					GetWorld(),
					BulletFlybySound,
					BulletLocaiton,
					1.0f,
					1.0f,
					2.0f
				);
				bPlayedSound = true;
			}
			else
			{
				//PRINTLOG(TEXT("거리가 멀어서 총알 소리가 들리진 않음"))
			}
		}
		else
		{
			//PRINTLOG(TEXT("이미 BulletFly 사운드 플레이함"))
		}
	}
}

float USG_ProjectileSystem::DragForce() const
{
	return -(AirDensity * FMath::Square(BulletSpeed) * CrossSection * DragCoefficiency ) / 2;
}

