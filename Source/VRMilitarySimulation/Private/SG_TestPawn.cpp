// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_TestPawn.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASG_TestPawn::ASG_TestPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASG_TestPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASG_TestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASG_TestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

