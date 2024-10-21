// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/MilitaryVRPawn.h"

// Sets default values
AMilitaryVRPawn::AMilitaryVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMilitaryVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

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

