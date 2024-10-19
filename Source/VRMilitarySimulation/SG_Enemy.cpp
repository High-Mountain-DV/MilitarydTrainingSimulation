// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASG_Enemy::ASG_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(RootComponent);
	GunMesh->SetRelativeTransform(FTransform(FRotator(-90, -90, 0) ,FVector(62.589846, 0.000002, 36.728229), FVector(0.01)));

	FirePosition = CreateDefaultSubobject<UArrowComponent>(TEXT("FirePosition"));
	FirePosition->SetupAttachment(GunMesh);
	FirePosition->SetRelativeTransform(FTransform(FRotator(0, 90, 90), FVector(-727.177226, 4335.648380, 305.000258), FVector(1))); 

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh);
	Muzzle->SetRelativeTransform(FTransform(FRotator(0), FVector(-727.177226, 4335.648380, 305.000258), FVector(1)));
}

// Called when the game starts or when spawned
void ASG_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
	BulletCount = MaxBulletCount;
}

// Called every frame
void ASG_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	// 총알 소환
	float PitchMin = -2, PitchMax = 5, YawMin = -2, YawMax = 2;
	FVector SpawnLocation = FirePosition->GetComponentLocation();
	FRotator SpawnRotation = FirePosition->GetComponentRotation() + FRotator(UKismetMathLibrary::RandomFloatInRange(PitchMin, PitchMax), UKismetMathLibrary::RandomFloatInRange(YawMin, YawMax), 0);
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	check(BP_EnemyBullet); if (nullptr == BP_EnemyBullet) return true;

	GetWorld()->SpawnActor<AActor>(BP_EnemyBullet, FTransform(SpawnRotation, SpawnLocation, FVector(1)), params);

	MulticastRPC_SpawnFireVFX(Muzzle->GetComponentTransform());

	BulletCount--;
	if (BulletCount) return true;
	else return false;
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

void ASG_Enemy::Reloading()
{
	BulletCount = MaxBulletCount;
}

void ASG_Enemy::MulticastRPC_SpawnFireVFX_Implementation(const FTransform& SpawnTransform)
{
	check(FireVFX); if (nullptr == FireVFX) return;

	// 격발 이펙트 소환
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireVFX, SpawnTransform);
}
