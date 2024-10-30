// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/MilitaryVRPawn.h"
#include "Net/UnrealNetwork.h"
#include "MotionControllerComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "TimerManager.h"

// Sets default values
AMilitaryVRPawn::AMilitaryVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//void AMilitaryVRPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AMilitaryVRPawn, RightController);
//	DOREPLIFETIME(AMilitaryVRPawn, LeftController);
//}

// Called when the game starts or when spawned
void AMilitaryVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//if (IsLocallyControlled())
	//{
	//	// 타이머를 설정하여 주기적으로 컨트롤러 Transform을 업데이트
	//	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle,
	//		[this]()
	//		{
	//			if (RightController && LeftController)
	//			{
	//				Server_UpdateControllerTransform(
	//					RightController->GetComponentTransform(),
	//					LeftController->GetComponentTransform()
	//				);
	//			}
	//		},
	//		0.1f, // 업데이트 주기 (초)
	//		true
	//	);
	//}
}

//void AMilitaryVRPawn::Server_UpdateControllerTransform_Implementation(
//	const FTransform& RightTransform,
//	const FTransform& LeftTransform)
//{
//	if (!HasAuthority()) return;
//
//	Multicast_UpdateControllerTransform(RightTransform, LeftTransform);
//}
//
//void AMilitaryVRPawn::Multicast_UpdateControllerTransform_Implementation(
//	const FTransform& RightTransform,
//	const FTransform& LeftTransform)
//{
//	if (IsLocallyControlled()) return;
//
//	if (RightController)
//	{
//		RightController->SetWorldTransform(RightTransform);
//	}
//	if (LeftController)
//	{
//		LeftController->SetWorldTransform(LeftTransform);
//	}
//}

// Called every frame
void AMilitaryVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMilitaryVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMilitaryVRPawn::DamageProcess(float Damage)
{
	// 적군의 총알에 두 발 맞으면 죽음 (프로토)
	p_hp -= Damage;
	if (p_hp == 0)
	{
		// 카메라 시점을 지휘관 카메라로 변경
		// 블루프린트로 작성 (지휘관 카메라가 블프로 되어있음)
	}
	//////////////////////////////////////////////////////////
	// (알파)
	// 한 다리 맞으면 시속3km/h 속도로 걷기
	// 두 다리 맞으면 앉은 상태로 총 겨누고 있기.
	// 총알이 팔에 맞으면, 그 팔의 컨트롤러  계속 진동오게 하기.해당 팔 차렷자세로 있기.
	// 두 팔 다 맞으면, 총 사용 못하고 이동만 가능하기
	// 몸통 맞으면 사망.

}

