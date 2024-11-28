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
#include "SG_Grenade.h"
#include "Engine/EngineTypes.h"
#include "CSW/CSWGameMode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"


// Sets default values
ASG_Enemy::ASG_Enemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CustomMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CustomMesh"));
	CustomMesh->SetupAttachment(GetMesh());

	static ConstructorHelpers::FClassFinder<ASG_WeaponMaster>tempWeaponClass(TEXT("/Game/MilitarySimulator/JSG/Blueprints/BP_AssaultRifle"));
	if (tempWeaponClass.Succeeded())
	{
		WeaponClass = tempWeaponClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("WeaponClass load Success"));
	}
	WeaponComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponComp"));
	if (WeaponClass)
	{
		WeaponComp->SetChildActorClass(WeaponClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponClass is nullptr"));
	}
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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempReloadMontage(TEXT("/Script/Engine.AnimMontage'/Game/MilitarySimulator/JSG/Animations/Weapon/AM_Rifle_Reload.AM_Rifle_Reload'"));
	if (tempReloadMontage.Succeeded())
	{
		MontageArray.Add(tempReloadMontage.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempThrowGrenadeMontage(TEXT("/Script/Engine.AnimMontage'/Game/MilitarySimulator/JSG/Animations/Weapon/AM_Grenade_Throw.AM_Grenade_Throw'"));
	if (tempThrowGrenadeMontage.Succeeded())
	{
		MontageArray.Add(tempThrowGrenadeMontage.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempTossGrenadeMontage(TEXT("/Script/Engine.AnimMontage'/Game/MilitarySimulator/JSG/Animations/Weapon/AM_Grenade_Toss.AM_Grenade_Toss'"));
	if (tempTossGrenadeMontage.Succeeded())
	{
		MontageArray.Add(tempTossGrenadeMontage.Object);
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel9, ECR_Overlap);
}



// Called when the game starts or when spawned
void ASG_Enemy::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
	{
		// 모든 디버그 메시지 비활성화
	GEngine->bEnableOnScreenDebugMessages = false;
	
		// 기존 메시지 제거
		GEngine->ClearOnScreenDebugMessages();
	}
	HP = MaxHP;
	// SetActorLabel(TEXT("Enemy"));

	Anim = Cast<USG_EnemyAnimInstance>(GetMesh()->GetAnimInstance());
	check(Anim); if (nullptr == Anim) return;

	PRINTLOG(TEXT("Anim 획득 성공"));
	SetWeapon();

	if (PathFindDebug)
	{
		DebugArrow->SetHiddenInGame(false);
	}

	GM = Cast<ACSWGameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ASG_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAutoMoveActive)
	{
		MoveTo_AutoMove(DeltaTime);
	}

	if (bAiming)
	{
		LerpAimoffset(DeltaTime);
	}
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("SpeedScale: %f"), SpeedScale));

	//LeftHandPos = CurrentWeapon->Weapon->GetSocketTransform(TEXT("LeftHandPosSocket")).GetLocation();
}

void ASG_Enemy::SetBehaviorTreeComponent(class UBehaviorTreeComponent* NewComp)
{
	BehaviorTreeComp = NewComp;
	Blackboard = NewComp->GetBlackboardComponent();
}

// Called to bind functionality to input
void ASG_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASG_Enemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASG_Enemy, CurrentWeapon);
	DOREPLIFETIME(ASG_Enemy, hp);
	DOREPLIFETIME(ASG_Enemy, DestinationAimPitch);
	DOREPLIFETIME(ASG_Enemy, DestinationAimYaw);
	DOREPLIFETIME(ASG_Enemy, bAiming);
	DOREPLIFETIME(ASG_Enemy, AimYaw);
	DOREPLIFETIME(ASG_Enemy, AimPitch);

}

const TMap<FString, struct TTuple<int32, float>> ASG_Enemy::GetHitLog() const
{
	return HitLog;
}

void ASG_Enemy::SetWeapon()
{
	if (!HasAuthority()) return;
	CurrentWeapon = Cast<ASG_WeaponMaster>(WeaponComp->GetChildActor());

	PRINTLOG(TEXT("WeaponComp->GetChildActor's Name: {%s}"), *CurrentWeapon->GetName());
	OnRep_CurrentWeapon();
}

void ASG_Enemy::ServerRPC_PlayAnimMontage(EEnemyAnimMontageType MontageType, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	check(Anim); if (nullptr == Anim) return;

	int32 MontageIdx = static_cast<int32>(MontageType);
	UE_LOG(LogTemp, Warning, TEXT("MontageIdx: %d"), MontageIdx);
	MulticastRPC_PlayAnimMontage(MontageArray[MontageIdx], InPlayRate, StartSectionName);
}

void ASG_Enemy::ServerRPC_PlayAnimMontage(class UAnimMontage* MontageToPlay, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	check(Anim); if (nullptr == Anim) return;

	MulticastRPC_PlayAnimMontage(MontageToPlay, InPlayRate, StartSectionName);
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

void ASG_Enemy::Aim(const FVector& AimTargetLocation)
{
	bAiming = true;
	FVector WeaponLocation = CurrentWeapon->FirePosition->GetComponentLocation();
	FVector TargetDirection = (AimTargetLocation - WeaponLocation).GetSafeNormal();

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

bool ASG_Enemy::FindPathPoints(const FVector& _TargetLocation, float Radius)
{
	PRINTLOG(TEXT(""));
	PointIndex = 1;
	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), _TargetLocation);
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
	bAutoMoveActive = true;
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

bool ASG_Enemy::ArriveAtLocation(const FVector& CurrLocation, const FVector& TargetLocation, float _AcceptableRadius)
{
	float Dist = FVector::Distance(CurrLocation, FVector(TargetLocation.X, TargetLocation.Y, CurrLocation.Z));
	return (Dist <= _AcceptableRadius);
}

void ASG_Enemy::HideWeaponMagazine()
{
	check(CurrentWeapon); if (nullptr == CurrentWeapon) return;
	CurrentWeapon->HideMagazine();
}

void ASG_Enemy::ShowWeaponMagazine()
{
	check(CurrentWeapon); if (nullptr == CurrentWeapon) return;
	CurrentWeapon->ShowMagazine();
}

FVector ASG_Enemy::GetThrowVelocityToTarget(const FVector& _GrenadeTargetPoint)
{
	// 1. 입력 유효성 검사
	if (!Grenade)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid actor references"));
		return FVector::ZeroVector;
	}

	// 2. 시작 위치 결정
	FVector CurLocation = Grenade ? Grenade->GetActorLocation() : GetActorLocation();

	// 3. 전체 변위 계산 (높이 차이 포함)
	FVector Displacement = _GrenadeTargetPoint - CurLocation;
	float TotalDistance = Displacement.Size();

	// 4. 최소 거리 체크
	const float MinDistance = 10.0f;
	if (TotalDistance < MinDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target too close, distance: %f"), TotalDistance);
		return FVector::ZeroVector;
	}

	// 5. 수평 거리와 높이 차이 계산
	float HorizontalDistance = FVector(Displacement.X, Displacement.Y, 0.0f).Size();
	float HeightDifference = Displacement.Z;

	// 6. 물리 상수
	const float Gravity = -980.0f;  // 실제 중력 가속도 (cm/s^2)

	// 7. 시간 계산 개선
	float EstimatedTime;
	if (TimeMultiplier > 0)
	{
		// 거리에 따른 비선형 시간 계산
		EstimatedTime = FMath::Sqrt(2.0f * HorizontalDistance / FMath::Abs(Gravity)) * TimeMultiplier;
		EstimatedTime = FMath::Clamp(EstimatedTime, 0.1f, 10.0f);  // 합리적인 범위로 제한
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid TimeMultiplier"));
		return FVector::ZeroVector;
	}

	// 8. 속도 계산
	float VerticalVelocity;
	float HorizontalVelocity;

	// 수직 속도 계산 (높이 차이 고려)
	VerticalVelocity = (HeightDifference - (0.5f * Gravity * EstimatedTime * EstimatedTime)) / EstimatedTime;

	// 수평 속도 계산
	HorizontalVelocity = HorizontalDistance / EstimatedTime;


	// 9. 최종 속도 벡터 계산
	FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, _GrenadeTargetPoint);
	FVector HorizontalVelocityVector = FVector(
		DirectionUnitVector.X * HorizontalVelocity,
		DirectionUnitVector.Y * HorizontalVelocity,
		VerticalVelocity
	);

	// 10. 속도 제한
	const float MaxVelocity = 10000.0f;  // 최대 속도 제한 (cm/s)
	if (HorizontalVelocityVector.Size() > MaxVelocity)
	{
		UE_LOG(LogTemp, Warning, TEXT("Velocity exceeded maximum limit, clamping"));
		HorizontalVelocityVector = HorizontalVelocityVector.GetClampedToSize(0.0f, MaxVelocity);
	}

	// 11. 디버그 정보
	UE_LOG(LogTemp, Log, TEXT("Throw Calculation: Distance=%f, Time=%f, Velocity=%s"),
		TotalDistance, EstimatedTime, *HorizontalVelocityVector.ToString());

	HorizontalVelocityVector *= FMath::RandRange(0.8f, 0.95f);

	return HorizontalVelocityVector;
}

void ASG_Enemy::ThrowGrenadeNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UE_LOG(LogTemp, Warning, TEXT("ASG_Enemy::ThrowGrenadeNotifyBegin"));
	if (NotifyName == TEXT("ThrowGrenade"))
	{
		check(Anim); if (nullptr == Anim) return;
		check(Grenade); if (nullptr == Grenade) return;

		FVector Velocity = ASG_Grenade::GetThrowVelocityToTarget(Grenade->GetActorLocation(), GrenadeTargetPoint, TimeMultiplier);
		MulticastRPC_ThrowGrenade(Velocity);

		Anim->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASG_Enemy::ThrowGrenadeNotifyBegin);
	}
}

void ASG_Enemy::MulticastRPC_ThrowGrenade_Implementation(const FVector& GrenadeVelocity)
{
	check(Grenade); if (nullptr == Grenade) return;

	Grenade->Active(this);
	Grenade->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//PRINTLOG(TEXT("GrenadeVelocity: %s"), *GrenadeVelocity.ToString());
	Grenade->BaseMesh->AddImpulse(GrenadeVelocity, NAME_None, true);
}

void ASG_Enemy::OnGrenadeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("OnGrenadeMontageEnded"));
	int32 ThrowGrenade = static_cast<int32>(EEnemyAnimMontageType::Throw_Grenade);
	int32 TossGrenade = static_cast<int32>(EEnemyAnimMontageType::Toss_Grenade);
	if (Montage == MontageArray[ThrowGrenade] || Montage == MontageArray[TossGrenade])
	{
		check(Anim); if (nullptr == Anim) return;
		check(CurrTask); if (nullptr == CurrTask) return;

		Anim->OnMontageEnded.RemoveDynamic(this, &ASG_Enemy::OnGrenadeMontageEnded);
		CurrTask->FinishLatentTask(*BehaviorTreeComp, EBTNodeResult::Succeeded);
		CurrTask = nullptr;
	}
}

ASG_WeaponMaster* ASG_Enemy::GetWeapon()
{
	return CurrentWeapon;
}

void ASG_Enemy::MoveTo_AutoMove(float DeltaTime)
{
	if (bDebugMoveTask)
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), NextTargetLocation, FVector(30), FColor::Purple, FRotator::ZeroRotator, DeltaTime);
	DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), NextTargetLocation);

	if (bFaceToDirection)
	{
		// 현재 액터의 rotation을 구합니다
		FRotator CurrentRotation = GetActorRotation();

		// DirectionVector를 회전값으로 변환
		FRotator TargetRotation = DirectionVector.Rotation();

		// 현재 회전값에서 목표 회전값으로 부드럽게 보간
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,    // 현재 회전값
			TargetRotation,     // 목표 회전값
			DeltaTime,         // 델타 타임
			2.0f              // 회전 속도 (이 값을 조절하여 회전 속도 변경)
		);
		// 새로운 회전값 적용
		NewRotation.Pitch = 0;
		SetActorRotation(NewRotation);
	}

	if (SpeedScale < 0.75 && FVector::Dist(GetActorLocation(), TargetLocation) <= 75 + AcceptableRadius)
	{
		SpeedScale = FMath::Max(0.3, SpeedScale - 0.025);
	}

	AddMovementInput(DirectionVector, SpeedScale);
	if (GetVelocity().Equals(FVector(0), 100))
	{
		ZeroVelocityCount++;
		if (ZeroVelocityCount >= 60)
		{
			float randomRangeValue = 200;
			float delta_x = FMath::RandRange(-randomRangeValue, randomRangeValue);
			float delta_y = FMath::RandRange(-randomRangeValue, randomRangeValue);

			NextTargetLocation.X = GetActorLocation().X + delta_x;
			NextTargetLocation.Y = GetActorLocation().Y + delta_y;

			if (false == bCanMove) PointIndex -= 1;
			bCanMove = true;

			ZeroVelocityCount = 0;
			if (bDebugMoveTask)
				UKismetSystemLibrary::DrawDebugBox(GetWorld(), NextTargetLocation, FVector(30), FColor::Purple, FRotator::ZeroRotator, DeltaTime);
		}
	}
	if (ArriveAtLocation(GetActorLocation(), NextTargetLocation, TempAcceptableRadius))
	{
		PointIndex += 1;
		ZeroVelocityCount = 0;
		bCanMove = false;
		if (PointIndex < PathPoints.Num())
		{
			NextTargetLocation = PathPoints[PointIndex];
			if (PointIndex == PathPoints.Num() - 1)
			{
				TempAcceptableRadius = AcceptableRadius;
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("TempAcceptableRadius Set! : %f"), AcceptableRadius));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Arrive at location"));
			bAutoMoveActive = false;
			DirectionVector = GetActorForwardVector();

			if (bRemoveTargetLocationKey)
			{
				check(Blackboard); if (nullptr == Blackboard) return;

				Blackboard->ClearValue(TargetLocationKeyID);
				TargetLocationKeyID = FBlackboard::FKey();
			}
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

void ASG_Enemy::UpdateHitLog(const float Damage, const FString& ShooterID)
{
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
}

void ASG_Enemy::AttachWeapon(const FName& SocketName)
{
	FAttachmentTransformRules const Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	WeaponComp->AttachToComponent(GetMesh(), Rules, SocketName);
}

void ASG_Enemy::SpawnAndGrabGrenade(const FName& SocketName)
{
	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	FTransform GrenadeSocketTransform = CustomMesh->GetSocketTransform(TEXT("Enemy_Grenade_Socket"));
	Grenade = GetWorld()->SpawnActor<ASG_Grenade>(BP_Grenade, GrenadeSocketTransform, params);
	check(Grenade); if (nullptr == Grenade) return;
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	Grenade->AttachToComponent(CustomMesh, rules, SocketName);
	Grenade->BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Grenade->BaseMesh->SetSimulatePhysics(false);
}

void ASG_Enemy::SetTargetLocationKeyName(const FBlackboard::FKey KeyID)
{
	TargetLocationKeyID = KeyID;
}

void ASG_Enemy::OnEnemyDetectGrenade(const FVector DetectedGrenadePoint)
{
	PRINTLOG(TEXT(""));
	check(Blackboard); if (nullptr == Blackboard) return;

	PRINTLOG(TEXT("수류탄 감지"));
	Blackboard->SetValueAsBool(TEXT("bDetectGrenade"), true);
	Blackboard->SetValueAsVector(TEXT("DetectedGrenadePoint"), DetectedGrenadePoint);
}

void ASG_Enemy::OnRep_CurrentWeapon()
{
	check(CurrentWeapon); if (nullptr == CurrentWeapon) return;

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
	FString ShooterID;
	if (Shooter) ShooterID = Shooter->Tags[1].ToString();
	UE_LOG(LogTemp, Warning, TEXT("ShooterID: %s"), *ShooterID);

	ServerRPC_DamageProcess(Damage, ShotDirection, ShooterID);
}

void ASG_Enemy::ServerRPC_DamageProcess_Implementation(float Damage, const FVector& ShotDirection, const FString& ShooterID)
{
	float DeltaHP = HP;
	HP -= Damage;
	DeltaHP -= HP;
	UpdateHitLog(DeltaHP, ShooterID);

	if (HP == 0)
	{
		DieProcess(ShotDirection, ShooterID);
	}
}
void ASG_Enemy::DieProcess(const FVector& ShotDirection, const FString& ShooterID)
{
	DetachFromControllerPendingDestroy();

	check(Blackboard); if (nullptr == Blackboard) return;

	if (Blackboard)
	{
		// bDead 값을 true로 설정
		Blackboard->SetValueAsBool(TEXT("bDead"), true);
	}

	WeaponComp->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	CurrentWeapon->Weapon->SetSimulatePhysics(true);
	CurrentWeapon->Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	FTransform spawnTransform = GetActorTransform();
	spawnTransform.SetLocation(spawnTransform.GetLocation() - FVector(0, 0, 5));

	MulticastRPC_SpawnDummyEnemy(spawnTransform, ShotDirection);

	if (GM)	GM->AppendHitLog(HitLog, ShooterID);
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