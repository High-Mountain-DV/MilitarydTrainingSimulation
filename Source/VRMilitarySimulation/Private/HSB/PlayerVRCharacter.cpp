// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/PlayerVRCharacter.h"
#include "MotionControllerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/CoreNet.h"

// Sets default values
APlayerVRCharacter::APlayerVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerVRCharacter, RightController);
	DOREPLIFETIME(APlayerVRCharacter, LeftController);
}

// Called when the game starts or when spawned
void APlayerVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled())
	{
		// 타이머를 설정하여 주기적으로 컨트롤러 Transform을 업데이트
		GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle,
			[this]()
			{
				if (RightController && LeftController)
				{
					Server_UpdateControllerTransform(
						RightController->GetComponentTransform(),
						LeftController->GetComponentTransform()
					);
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

	Multicast_UpdateControllerTransform(RightTransform, LeftTransform);
}

void APlayerVRCharacter::Multicast_UpdateControllerTransform_Implementation(
	const FTransform& RightTransform,
	const FTransform& LeftTransform)
{
	if (IsLocallyControlled()) return;

	if (RightController)
	{
		RightController->SetWorldTransform(RightTransform);
	}
	if (LeftController)
	{
		LeftController->SetWorldTransform(LeftTransform);
	}
}

// Called every frame
void APlayerVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

