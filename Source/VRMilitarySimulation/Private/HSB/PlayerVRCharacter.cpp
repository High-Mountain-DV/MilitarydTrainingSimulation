// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/PlayerVRCharacter.h"

// Sets default values
APlayerVRCharacter::APlayerVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

