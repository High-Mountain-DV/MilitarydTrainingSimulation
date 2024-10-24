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
#include "VRMilitarySimulation.h"
#include "SG_EnemyAnimInstance.h"


// Sets default values
ASG_Enemy::ASG_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponComp"));
	//WeaponComp->SetRelativeLocation(FVector(62.589846, 0.000002, 36.728229)); 
	WeaponComp->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MilitarySimulator/SHN/Assets/Character/NorthKorean/sol_8_low.sol_8_low'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	DebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DebugArrow"));
	DebugArrow->SetupAttachment(RootComponent);
}



// Called when the game starts or when spawned
void ASG_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
	Anim = Cast<USG_EnemyAnimInstance>(GetMesh()->GetAnimInstance());
	check(Anim); if (nullptr == Anim) return;

	BulletCount = MaxBulletCount;
	SetWeapon();
	check(CurrentWeapon); if (nullptr == CurrentWeapon) return;
	CurrentWeapon->SetOwner(this);
	UE_LOG(LogTemp, Warning, TEXT("WeaponComp->GetChildActor's Name: {%s}"), *CurrentWeapon->GetName());
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
		AI_Move_To(DeltaTime);
	}

	if (bAiming)
	{
		LerpAimoffset(DeltaTime);
	}
	else
	{
		/*AimPitch = FMath::Lerp(AimPitch, 0, DeltaTime * 5);
		AimYaw = FMath::Lerp(AimYaw, 0, DeltaTime * 5);*/
	}
	LeftHandPos = CurrentWeapon->Weapon->GetSocketTransform(TEXT("LeftHandPosSocket")).GetLocation();
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

void ASG_Enemy::SetWeapon()
{
	//CurrentWeapon = GetWorld()->SpawnActor<ASG_WeaponMaster>(WeaponClass, GetMesh()->GetSocketTransform(TEXT("Enemy_TwoHandedGun_Socket")), param);
	//check(CurrentWeapon); if (nullptr == CurrentWeapon) return;
	//CurrentWeapon->K2_AttachToComponent(GetMesh(), TEXT("Enemy_TwoHandedGun_Socket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

	////CurrentWeapon->AttachToComponent(GetMesh(), Rules, TEXT("Enemy_TwoHandedGun_Socket"));

	FAttachmentTransformRules const Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	WeaponComp->AttachToComponent(GetMesh(), Rules, TEXT("Enemy_TwoHandedGun_Socket"));
	if (WeaponClass) WeaponComp->SetChildActorClass(WeaponClass);
	WeaponComp->CreateChildActor();
	CurrentWeapon = Cast<ASG_WeaponMaster>(WeaponComp->GetChildActor());
	check(CurrentWeapon); if (nullptr == CurrentWeapon) return;

	CurrentWeapon->SetOwner(this);
	CurrentWeapon->SetInstigator(this);
	CurrentWeapon->Weapon->SetVisibility(true);
	CurrentWeapon->SetShooter();
	Anim->Weapon = CurrentWeapon;
}

bool ASG_Enemy::Fire(bool& OutStopShooting)
{
	bool bMagazineEmpty = CurrentWeapon->Fire(OutStopShooting);

	float deltaPitch = UKismetMathLibrary::RandomFloatInRange(4, 8);
	float deltaYaw = UKismetMathLibrary::RandomFloatInRange(-2, 2);
	DestinationAimPitch += deltaPitch;
	DestinationAimYaw += deltaYaw;
	AimPitch += deltaPitch;
	AimYaw += deltaYaw;
	
	return bMagazineEmpty;
}

void ASG_Enemy::Aim(const FVector TargetLocation)
{
	bAiming = true;
	FVector WeaponLocation = CurrentWeapon->FirePosition->GetComponentLocation();
	FVector TargetDirection = (TargetLocation - WeaponLocation).GetSafeNormal();

	// 현재 Actor의 Forward 방향을 기준으로 상대적인 회전값을 계산
	FVector ActorForward = GetActorForwardVector();
	FVector ActorRight = GetActorRightVector();
	FVector ActorUp = GetActorUpVector();

	// Yaw 계산 (수평 회전)
	float DotProduct = FVector::DotProduct(ActorForward, TargetDirection);
	float RightDot = FVector::DotProduct(ActorRight, TargetDirection);
	DestinationAimYaw = FMath::Atan2(RightDot, DotProduct) * 180.0f / PI;

	if (DestinationAimYaw > 0) DestinationAimYaw += 2.31;
	else DestinationAimYaw += 4.81;

	// Pitch 계산 (수직 회전)
	float UpDot = FVector::DotProduct(ActorUp, TargetDirection);
	float ForwardLength = FVector::VectorPlaneProject(TargetDirection, ActorUp).Size();
	DestinationAimPitch = FMath::Atan2(UpDot, ForwardLength) * 180.0f / PI * AimOffsetPitchCoef;

	// 각도 제한 (필요한 경우)
	DestinationAimPitch = FMath::ClampAngle(DestinationAimPitch, -90.0f, 90.0f);
	DestinationAimYaw = FMath::ClampAngle(DestinationAimYaw, -90.0f, 90.0f);

	//UE_LOG(LogTemp, Warning, TEXT("DestinationAimPitch: {%f}, DestinationAimYaw: {%f}"), DestinationAimPitch, DestinationAimYaw);

}


void ASG_Enemy::Reloading()
{
	CurrentWeapon->Reloading();
}

bool ASG_Enemy::FindPathPoints(const FVector& TargetLocation, float Radius)
{
	PRINTLOG(TEXT(""));
	PointIndex = 1;
	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), TargetLocation);
	if (!Path)
	{
		UE_LOG(LogTemp, Warning, TEXT("!Path"));
		return false;
	}

	PathPoints = Path->PathPoints;
	if (PathPoints.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path is not exist"));
		return false;
	}
	NextTargetLocation = PathPoints[PointIndex];
	if (PathFindDebug)
	{
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), NextTargetLocation, FVector(15), FColor::Purple, FRotator::ZeroRotator, 10);
	}
	Speed = 0.95;
	StartMovement = true;
	AcceptableRadius = Radius;
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
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, FVector(10), FColor::Blue, FRotator::ZeroRotator, 10);
	}
}

FVector ASG_Enemy::GetDirectionToTarget()
{
	FVector fromLocation = GetActorLocation();
	FVector ToLocation = FVector(NextTargetLocation.X, NextTargetLocation.Y, fromLocation.Z);

	return UKismetMathLibrary::GetDirectionUnitVector(fromLocation, ToLocation);
}

bool ASG_Enemy::ArriveAtLocation(FVector Location)
{
	float Dist = FVector::Distance(GetActorLocation(), FVector(Location.X, Location.Y, GetActorLocation().Z));
	return (Dist <= AcceptableRadius);
}

void ASG_Enemy::StopMovement()
{
	Speed = 0;
	StartMovement = 0;
	DirectionVector = GetActorForwardVector();
}

void ASG_Enemy::AI_Move_To(float DeltaTime)
{
	AimPitch = FMath::Lerp(AimPitch, 0, DeltaTime * 10);
	AimYaw = FMath::Lerp(AimYaw, 0, DeltaTime * 10);

	DirectionVector = GetDirectionToTarget();

	// 현재 액터의 rotation을 구합니다
	FRotator CurrentRotation = GetActorRotation();

	// DirectionVector를 회전값으로 변환
	FRotator TargetRotation = DirectionVector.Rotation();

	// 현재 회전값에서 목표 회전값으로 부드럽게 보간
	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,    // 현재 회전값
		TargetRotation,     // 목표 회전값
		DeltaTime,         // 델타 타임
		5.0f              // 회전 속도 (이 값을 조절하여 회전 속도 변경)
	);

	// 새로운 회전값 적용
	SetActorRotation(NewRotation);

	DebugArrow->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(DirectionVector));
	AddMovementInput(DirectionVector, Speed);
	if (ArriveAtLocation(NextTargetLocation))
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

void ASG_Enemy::LerpAimoffset(float DeltaTime)
{
	AimPitch = FMath::Lerp(AimPitch, DestinationAimPitch, DeltaTime * 12);
	AimYaw = FMath::Lerp(AimYaw, DestinationAimYaw, DeltaTime * 12);

	//UE_LOG(LogTemp, Warning, TEXT("AimPitch: {%f}, AimYaw: {%f}"), AimPitch, AimYaw);

	if (FMath::Abs(AimPitch - DestinationAimPitch) <= 0.1 && FMath::Abs(AimYaw - DestinationAimYaw) <= 0.1)
	{
		bAiming = false;
	}
}

void ASG_Enemy::OnRep_HP()
{
	if (HP <= 0)
	{
		bDead = true;
		//GetCharacterMovement()->DisableMovement();
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
