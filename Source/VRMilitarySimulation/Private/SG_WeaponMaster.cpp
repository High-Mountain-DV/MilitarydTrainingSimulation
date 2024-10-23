// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_WeaponMaster.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../VRMilitarySimulation.h"

// Sets default values
ASG_WeaponMaster::ASG_WeaponMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(BoxComp);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FirePosition = CreateDefaultSubobject<UArrowComponent>(TEXT("FirePosition"));
	FirePosition->SetupAttachment(Weapon);
	FirePosition->SetRelativeLocation(FVector(-749.233604, 351.409237, 292.697012));
	FirePosition->SetRelativeRotation(FRotator(0, 90, 0));

	MuzzlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePosition"));
	MuzzlePosition->SetupAttachment(Weapon);
	MuzzlePosition->SetRelativeLocation(FVector(-718.212279, 4505.655099, 302.310963));
}

// Called when the game starts or when spawned
void ASG_WeaponMaster::BeginPlay()
{
	Super::BeginPlay();

	//Shooter = Cast<APawn>(GetOwner());
	//check(Shooter); if (nullptr == Shooter) return;
}

// Called every frame
void ASG_WeaponMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (bRLerpFlag)
	{
		RecoverRecoil(DeltaTime);
	}*/
}

void ASG_WeaponMaster::Aim(const FVector TargetLocation)
{
	PRINTLOG(TEXT(""));
	// 목표 방향 계산
	FVector GunLocation = GetActorLocation();
	FVector DirectionToTarget = (TargetLocation - GunLocation).GetSafeNormal();
	//UKismetSystemLibrary::DrawDebugLine(GetWorld(), GunLocation, TargetLocation, FColor::Blue, 3);
	// 회전값 계산
	AimRotation = DirectionToTarget.Rotation();
	//	bRLerpFlag = true;

		// GunMesh 회전 적용
	SetActorRotation(AimRotation);
}

bool ASG_WeaponMaster::Fire(bool& OutStopShooting)
{
	PRINTLOG(TEXT("Fire"));
	// 총알 소환
	FVector SpawnLocation = FirePosition->GetComponentLocation();
	FRotator SpawnRotation = FirePosition->GetComponentRotation() + FRotator(UKismetMathLibrary::RandomFloatInRange(PitchMin, PitchMax), UKismetMathLibrary::RandomFloatInRange(YawMin, YawMax), 0);
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//params.Instigator = Cast<APawn>(Shooter);
	check(BP_EnemyBullet); if (nullptr == BP_EnemyBullet) return true;

	//GetWorld()->SpawnActor<AActor>(BP_EnemyBullet, FTransform(SpawnRotation, SpawnLocation, FVector(1)), params);
	auto* bullet = GetWorld()->SpawnActor<AActor>(BP_EnemyBullet, FirePosition->GetComponentTransform(), params);

	MulticastRPC_SpawnFireVFX(MuzzlePosition->GetComponentTransform());

	Recoil();

	if (UKismetMathLibrary::RandomIntegerInRange(0, 100) > StopShootingProb)
	{
		OutStopShooting = false;
		StopShootingProb += StopShootingDelta;
		UE_LOG(LogTemp, Warning, TEXT("StopShootingProb: %d"), StopShootingProb);
	}
	else
	{
		OutStopShooting = true;
		StopShootingProb = 0;
	}

	BulletCount--;
	if (BulletCount) return true;
	else return false;
}

void ASG_WeaponMaster::Recoil()
{
	float DeltaPitch = UKismetMathLibrary::RandomFloatInRange(PitchMin, PitchMax);
	float DeltaYaw = UKismetMathLibrary::RandomFloatInRange(YawMin, YawMax);
	FRotator DeltaRotation(DeltaPitch, DeltaYaw, 0);
	AddActorLocalRotation(DeltaRotation);

	//// 이미 진행 중인 복구 타이머가 있다면 리셋
	//if (GetWorld()->GetTimerManager().IsTimerActive(RecoveryTimerHandle))
	//{
	//	GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);
	//}
	//// 딜레이 후 복구 시작
	//GetWorld()->GetTimerManager().SetTimer(
	//	RecoveryTimerHandle,
	//	[this]() { StartRecoilRecovery(); },
	//	RecoilRecoveryDelay,
	//	false
	//);
}

void ASG_WeaponMaster::StartRecoilRecovery()
{
	bRLerpFlag = true;

}

// 실제 복구 계산
void ASG_WeaponMaster::RecoverRecoil(float DeltaTime)
{
	// 현재 회전값
	FRotator CurrentRotation = GetActorRotation();

	// Lerp로 부드럽게 복구
	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		AimRotation,
		DeltaTime,
		RecoilRecoverySpeed
	);

	// 새로운 회전값 적용
	SetActorRotation(NewRotation);

	// 목표값에 충분히 가까워졌는지 체크
	if (CurrentRotation.Equals(AimRotation, 0.1f))
	{
		// 완전히 복구되면 상태 리셋
		bRLerpFlag = false;
		SetActorRotation(Owner->GetActorRotation());
	}

}

void ASG_WeaponMaster::Reloading()
{
	PRINTLOG(TEXT(""));
	BulletCount = MaxBulletCount;
}





void ASG_WeaponMaster::MulticastRPC_SpawnFireVFX_Implementation(const FTransform& SpawnTransform)
{
	check(FireVFX); if (nullptr == FireVFX) return;

	// 격발 이펙트 소환
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireVFX, SpawnTransform);
}

