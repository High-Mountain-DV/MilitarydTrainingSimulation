// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/PlayerVRCharacter.h"
#include "MotionControllerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/CoreNet.h"
#include "CSW/CSWGameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystemTypes.h"

// Sets default values
APlayerVRCharacter::APlayerVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerVRCharacter, RightCPPController);
	DOREPLIFETIME(APlayerVRCharacter, LeftCPPController);
}

// Called when the game starts or when spawned
void APlayerVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임 인스턴스에서 ID 가져오기
	UCSWGameInstance* GameInstance = Cast<UCSWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// 플레이어 ID를 액터 이름으로 설정
		// GetUserId()의 반환값을 FString으로 변환
		FString PlayerIdString(GameInstance->GetNickname());
		SetActorLabel(PlayerIdString);
		PlayerId = PlayerIdString;
	}

	if (IsLocallyControlled())
	{
		// 타이머를 설정하여 주기적으로 컨트롤러 Transform을 업데이트
		GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle,
			[this]()
			{
				UE_LOG(LogTemp, Warning, TEXT("Has Not Controlled"));
				if (RightCPPController && LeftCPPController)
				{
					Server_UpdateControllerTransform(
						RightCPPController->GetComponentTransform(),
						LeftCPPController->GetComponentTransform()
					);
					UE_LOG(LogTemp, Warning, TEXT("Has Controlled"));
				}
			},
			0.1f, // 업데이트 주기 (초)
			true
		);
	}
}

void APlayerVRCharacter::Server_UpdateControllerTransform_Implementation(
	const FTransform& RightTransform,
	const FTransform& LeftTransform)
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Im Server"));
	Multicast_UpdateControllerTransform(RightTransform, LeftTransform);
}

void APlayerVRCharacter::Multicast_UpdateControllerTransform_Implementation(
	const FTransform& RightTransform,
	const FTransform& LeftTransform)
{
	if (IsLocallyControlled()) return;

	if (RightCPPController)
	{
		RightCPPController->SetWorldTransform(RightTransform);
	}
	if (LeftCPPController)
	{
		LeftCPPController->SetWorldTransform(LeftTransform);
	}
}

// Called every frame
void APlayerVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector RightLoc;
		FVector LeftLoc;
		RightLoc = RightCPPController->GetComponentLocation();
		LeftLoc = LeftCPPController->GetComponentLocation();
		
//		UE_LOG(LogTemp, Warning, TEXT("X: %f , Y: %f, Z: %f"), RightLoc.X, RightLoc.Y, RightLoc.Z);
	}
}

// Called to bind functionality to input
void APlayerVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

int APlayerVRCharacter::GetShootingCnt() const
{
	return ShootingCnt;
}

