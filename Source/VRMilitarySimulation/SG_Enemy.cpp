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
#include "SG_EnemyAIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "SG_DummyEnemy.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SG_Grenede.h"
#include "Engine/EngineTypes.h"
#include "CSW/CSWGameMode.h"


// Sets default values
ASG_Enemy::ASG_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CustomMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CustomMesh"));
	CustomMesh->SetupAttachment(GetMesh());

	WeaponComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponComp"));
	//WeaponComp->SetRelativeLocation(FVector(62.589846, 0.000002, 36.728229)); 
	WeaponComp->SetupAttachment(GetMesh(), TEXT("Enemy_TwoHandedGun_Socket"));

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
	
	GM = Cast<ACSWGameMode>(GetWorld()->GetAuthGameMode());
	check(GM); if (nullptr == GM) return;


	Anim = Cast<USG_EnemyAnimInstance>(GetMesh()->GetAnimInstance());
	check(Anim); if (nullptr == Anim) return;

	PRINTLOG(TEXT("Anim 획득 성공"));
	SetWeapon();

	if (PathFindDebug)
	{
		DebugArrow->SetHiddenInGame(false);
	}

	//if (GEngine)
	//{
	//	// 모든 디버그 메시지 비활성화
	//	GEngine->bEnableOnScreenDebugMessages = false;

	//	// 기존 메시지 제거
	//	GEngine->ClearOnScreenDebugMessages();
	//}
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
	//LeftHandPos = CurrentWeapon->Weapon->GetSocketTransform(TEXT("LeftHandPosSocket")).GetLocation();
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
	DOREPLIFETIME(ASG_Enemy, CurrentWeapon);
	DOREPLIFETIME(ASG_Enemy, DestinationAimPitch);
	DOREPLIFETIME(ASG_Enemy, DestinationAimYaw);
	DOREPLIFETIME(ASG_Enemy, bAiming);
	DOREPLIFETIME(ASG_Enemy, AimYaw);
	DOREPLIFETIME(ASG_Enemy, AimPitch);
	
}

void ASG_Enemy::SetWeapon()
{
	if (!HasAuthority()) return;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FAttachmentTransformRules const Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	WeaponComp->AttachToComponent(GetMesh(), Rules, TEXT("Enemy_TwoHandedGun_Socket"));
	if (WeaponClass) WeaponComp->SetChildActorClass(WeaponClass);

	WeaponComp->CreateChildActor();
	CurrentWeapon = Cast<ASG_WeaponMaster>(WeaponComp->GetChildActor());

	OnRep_CurrentWeapon();
}

void ASG_Enemy::ServerRPC_PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	check(Anim); if (nullptr == Anim) return;

	MulticastRPC_PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

void ASG_Enemy::MulticastRPC_PlayAnimMontage_Implementation(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

bool ASG_Enemy::Fire(bool& OutStopShooting)
{
	bool bMagazineEmpty = CurrentWeapon->Fire(OutStopShooting);

	Recoil();
	
	return bMagazineEmpty;
}

void ASG_Enemy::Aim(const FVector TargetLocation)
{
	bAiming = true;
	FVector WeaponLocation = CurrentWeapon->FirePosition->GetComponentLocation();
	FVector TargetDirection = (TargetLocation - WeaponLocation).GetSafeNormal();

	// 현재 Actor의 Forward 방향을 기준으로 상대적인 회전값을 계산
	FVector ActorForward = GetActorForwardVector();
	FVector ActorUp = GetActorUpVector();

	// Pitch 계산 (수직 회전)
	float UpDot = FVector::DotProduct(ActorUp, TargetDirection);
	float ForwardLength = FVector::VectorPlaneProject(TargetDirection, ActorUp).Size();
	DestinationAimPitch = FMath::Atan2(UpDot, ForwardLength) * 180.0f / PI;

	// 각도 제한
	DestinationAimPitch = FMath::ClampAngle(DestinationAimPitch, -90.0f, 90.0f);
}


void ASG_Enemy::Reloading()
{
	CurrentWeapon->Reloading();
}

void ASG_Enemy::SetAimOffsetAlpha(float AimOffsetAlpha)
{
	MulticastRPC_SetAimOffsetAlpha(AimOffsetAlpha);
}

void ASG_Enemy::MulticastRPC_SetAimOffsetAlpha_Implementation(float AimOffsetAlpha)
{
	if (nullptr == Anim) 
	{
		PRINTLOG(TEXT("이거뜨면 Anim이 없어지는 버그인듯"));
		return;
	}
	Anim->AimOffsetAlpha = AimOffsetAlpha;
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
	StartMovement = false;
	DirectionVector = GetActorForwardVector();
}

void ASG_Enemy::HideWeaponMagazine()
{
	CurrentWeapon->HideMagazine();
}

void ASG_Enemy::ShowWeaponMagazine()
{
	CurrentWeapon->ShowMagazine();
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
		}
		else
		{
			StopMovement();
		}
	}
}

void ASG_Enemy::LerpAimoffset(float DeltaTime)
{
	AimPitch = FMath::Lerp(AimPitch, DestinationAimPitch, DeltaTime * 6);
	AimYaw = FMath::Lerp(AimYaw, DestinationAimYaw, DeltaTime * 6);
	//UE_LOG(LogTemp, Warning, TEXT("AimPitch: {%f}, AimYaw: {%f}"), AimPitch, AimYaw);

	if (FMath::Abs(AimPitch - DestinationAimPitch) <= 0.1 && FMath::Abs(AimYaw - DestinationAimYaw) <= 0.1)
	{
		bAiming = false;
	}
}

void ASG_Enemy::DieProcess(const FName& BoneName, const FVector& ShotDirection, AActor* Shooter)
{
	ServerRPC_DieProcess(BoneName, ShotDirection, Shooter);
}

void ASG_Enemy::ServerRPC_DieProcess_Implementation(const FName& BoneName, const FVector& ShotDirection, AActor* Shooter)
{
	DetachFromControllerPendingDestroy();
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			// BehaviorTreeComponent 가져오기
			UBehaviorTreeComponent* BehaviorTreeComp = AIController->FindComponentByClass<UBehaviorTreeComponent>();
			if (BehaviorTreeComp)
			{
				// Blackboard 가져오기
				UBlackboardComponent* Blackboard = BehaviorTreeComp->GetBlackboardComponent();
				if (Blackboard)
				{
					// bDead 값을 true로 설정
					Blackboard->SetValueAsBool(TEXT("bDead"), true);
				}
			}
		}
	}

	WeaponComp->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	CurrentWeapon->Weapon->SetSimulatePhysics(true);
	CurrentWeapon->Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	FTransform spawnTransform = GetActorTransform();
	spawnTransform.SetLocation(spawnTransform.GetLocation() - FVector(0, 0, 5));

	MulticastRPC_SpawnDummyEnemy(spawnTransform, ShotDirection);

	Destroy();
}

void ASG_Enemy::MulticastRPC_SpawnDummyEnemy_Implementation(const FTransform& SpawnTransform, const FVector& ShotDirection)
{
	float force = 10000;
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto* dummyEnemy = GetWorld()->SpawnActor<ASG_DummyEnemy>(BP_DummyEnemy, SpawnTransform, params);
	dummyEnemy->Mesh->AddImpulse(ShotDirection * force);
}

void ASG_Enemy::Recoil()
{
	float deltaPitch = UKismetMathLibrary::RandomFloatInRange(RecoilPitchMinOffset, RecoilPitchMaxOffset);
	float deltaYaw = UKismetMathLibrary::RandomFloatInRange(RecoilYawMinOffset, RecoilYawMaxOffset);

	if (!HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("AimPitch: {%f}, AimYaw: {%f}"), AimPitch, AimYaw);
	AimPitch += deltaPitch;
	AimYaw += deltaYaw;

	bAiming = true;
	DestinationAimPitch = 0;
	DestinationAimYaw = 0;
}

void ASG_Enemy::AttachWeapon(const FName& SocketName)
{
	FAttachmentTransformRules const Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	WeaponComp->AttachToComponent(GetMesh(), Rules, SocketName);
}

void ASG_Enemy::SpawnAndGrabGrenede(const FName& SocketName)
{
	FActorSpawnParameters params;
	params.Instigator = this;
	Grenede = GetWorld()->SpawnActor<ASG_Grenede>(BP_Grenede, CustomMesh->GetSocketTransform(TEXT("Enemy_Grenede_Socket")), params);
	check(Grenede); if (nullptr == Grenede) return;

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	Grenede->AttachToComponent(CustomMesh, rules, SocketName);

	Grenede->CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Grenede->CapsuleComp->SetSimulatePhysics(false);
	Grenede->SetOwner(this);
}

void ASG_Enemy::ThrowGrenede()
{
	Grenede->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Grenede->CapsuleComp->SetSimulatePhysics(true);
	Grenede->CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Grenede->Active(this);

	//Grenede->CapsuleComp->AddImpulse(GrenedeVelocity);
	bool bGrenedeThrow = Grenede->ThrowWithCheck(GrenedePoint);
	Grenede = nullptr;
}

void ASG_Enemy::OnRep_CurrentWeapon()
{
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->SetInstigator(this);
	CurrentWeapon->SetShooter();

	PRINTLOG(TEXT("WeaponComp->GetChildActor's Name: {%s}"), *CurrentWeapon->GetName());
}

void ASG_Enemy::OnRep_HP()
{
	//if (HP <= 0)
	//{
	//	bDead = true;
	//}
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

void ASG_Enemy::DamageProcess(float Damage, const FName& BoneName, const FVector& ShotDirection, AActor* Shooter)
{
	FString BoneString = BoneName.ToString();
	if (BoneName == TEXT("head") || BoneString.Contains(TEXT("neck")))
	{
		Damage *= HeadShotMultiplier;
		UE_LOG(LogTemp, Warning, TEXT("HeadShot!"));
	}
	else if (BoneString.Contains(TEXT("arm")) || BoneString.Contains(TEXT("calf")) || BoneString.Contains(TEXT("thigh")))
	{
		Damage *= ArmOrLegShotMultiplier;
		UE_LOG(LogTemp, Warning, TEXT("Arm or Leg Shot!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BodyShot!"));
		Damage *= BodyShotMultiplier;
	}

	ApplyImpactToBone(BoneName, ShotDirection);

	HP -= Damage;

	if (HP == 0)
	{
		DieProcess(BoneName, ShotDirection, Shooter);
	}

	
	FString ShooterID;
	if (Shooter) ShooterID = Shooter->GetActorLabel();
	UE_LOG(LogTemp, Warning, TEXT("ShooterID: %s"), *ShooterID);

	if (TTuple<int32, float>* ExistingLog = HitLog.Find(ShooterID))
	{
		ExistingLog->Get<0>() += 1;
		ExistingLog->Get<1>() += Damage;
		UE_LOG(LogTemp, Warning, TEXT("HitLog Update! %d, %f"), ExistingLog->Get<0>(), ExistingLog->Get<1>());
	}
	else
	{
		HitLog.Add(ShooterID, TTuple<int32, float>(1, Damage));
	}
	GM->AppendHitLog(HitLog);
}
