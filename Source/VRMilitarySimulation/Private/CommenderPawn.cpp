// Fill out your copyright notice in the Description page of Project Settings.


#include "CommenderPawn.h"

#include "Net/VoiceConfig.h"

// Sets default values
ACommenderPawn::ACommenderPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VOIPTalkerComponent = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));
}

// Called when the game starts or when spawned
void ACommenderPawn::BeginPlay()
{
	Super::BeginPlay();

	InitializeVOIP();
}

// Called every frame
void ACommenderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACommenderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACommenderPawn::RegisterWithPlayerState()
{
	if (VOIPTalkerComponent && GetPlayerState())
	{
		VOIPTalkerComponent->RegisterWithPlayerState(GetPlayerState());
	}
}

void ACommenderPawn::SetMicThreshold(float Threshold)
{
	if (VOIPTalkerComponent)
	{
		UVOIPStatics::SetMicThreshold(Threshold);
	}
}

void ACommenderPawn::InitializeVOIP()
{
	if (VOIPTalkerComponent)
	{
		// VOIPTalkerComponent가 유효한지 확인합니다.
		if (IsValid(VOIPTalkerComponent))
		{
			// 플레이어 상태에 VOIPTalker를 등록합니다.
			RegisterWithPlayerState();

			// 마이크 임계값을 설정합니다.
			SetMicThreshold(-1.0f);

			// 로컬 플레이어가 제어 중일 때만 VOIP 관련 설정을 진행합니다.
			if (IsLocallyControlled())
			{
				// 콘솔 명령을 실행하여 VOIP를 활성화합니다.
				APlayerController* PlayerController = Cast<APlayerController>(GetController());
				if (PlayerController)
				{
					PlayerController->ConsoleCommand("OSS.VoiceLoopback 1");
				}
			}
		}
	}
}