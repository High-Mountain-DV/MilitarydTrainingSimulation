// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SG_WeaponMaster.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"


// Sets default values
ASG_Enemy::ASG_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponComp"));
	WeaponComp->SetupAttachment(RootComponent);
	WeaponComp->SetRelativeLocation(FVector(62.589846, 0.000002, 36.728229));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MilitarySimulator/SHN/Assets/Character/NorthKorean/sol_8_low.sol_8_low'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	DebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DebugArrow"));
	DebugArrow->SetupAttachment(GetArrowComponent());
}

// Called when the game starts or when spawned
void ASG_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
	BulletCount = MaxBulletCount;
	CurrentWeapon = Cast<ASG_WeaponMaster>(WeaponComp->GetChildActor());
	check(CurrentWeapon); if (nullptr == CurrentWeapon) return;
	CurrentWeapon->SetOwner(this);

	if (PathFindDebug)
	{
		DebugArrow->SetHiddenInGame(false);
	}
}

// Called every frame
void ASG_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (StartMovement)
	{
		DirectionVector = GetDirectionToTarget();
		DebugArrow->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(DirectionVector));
		AddMovementInput(DirectionVector, Speed);
		if (ArriveAtLocation(NextTargetLocation, 50))
		{
			PointIndex += 1;
			if (PointIndex < PathPoints.Num())
			{
				NextTargetLocation = PathPoints[PointIndex];
				Speed = 0.95;
				StartMovement = true;
			}
			else
			{
				StopMovement();
			}
		}
	}
}

// Called to bind functionality to input
void ASG_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASG_Enemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASG_Enemy, hp);
}

bool ASG_Enemy::Fire()
{
	bool bMagazineEmpty = CurrentWeapon->Fire();
	return bMagazineEmpty;
}

void ASG_Enemy::Aim(const FVector TargetLocation)
{
	CurrentWeapon->Aim(TargetLocation);
}
void ASG_Enemy::Reloading()
{
	CurrentWeapon->Reloading();
}

bool ASG_Enemy::FindPathPoints(const FVector& TargetLocation)
{
	PointIndex = 1;
	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), TargetLocation);
	if (!Path) return false;

	PathPoints = Path->PathPoints;
	NextTargetLocation = PathPoints[PointIndex];
	if (PathFindDebug)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), NextTargetLocation, FVector(15), FColor::Red, FRotator::ZeroRotator, 10);
	}
	Speed = 0.95;
	StartMovement = true;

	if (PathFindDebug)
	{
		DebugPoints(PathPoints);
	}
	return true;
}

void ASG_Enemy::DebugPoints(const TArray<FVector>& Array)
{
	for (const FVector& point : Array)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, FVector(10), FColor::Green, FRotator::ZeroRotator, 10);
	}
}

FVector ASG_Enemy::GetDirectionToTarget()
{
	FVector fromLocation = GetActorLocation();
	FVector ToLocation = FVector(NextTargetLocation.X, NextTargetLocation.Y, fromLocation.Z);

	return UKismetMathLibrary::GetDirectionUnitVector(fromLocation, ToLocation);
}

bool ASG_Enemy::ArriveAtLocation(FVector Location, float Radius)
{
	float Dist = FVector::Distance(GetActorLocation(), FVector(Location.X, Location.Y, GetActorLocation().Z));
	return (Dist <= Radius);
}

void ASG_Enemy::StopMovement()
{
	Speed = 0;
	StartMovement = 0;
	DirectionVector = GetActorForwardVector();
}

void ASG_Enemy::OnRep_HP()
{
	if (HP <= 0)
	{
		bDead = true;
		GetCharacterMovement()->DisableMovement();
	}
}

float ASG_Enemy::GetHP()
{
	return hp;
}

void ASG_Enemy::SetHP(float Value)
{
	hp = FMath::Max(0, Value);
	OnRep_HP();
}

void ASG_Enemy::DamageProcess(float Damage)
{
	HP -= Damage;
}
