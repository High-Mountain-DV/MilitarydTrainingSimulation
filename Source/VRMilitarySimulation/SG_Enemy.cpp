// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Enemy.h"

// Sets default values
ASG_Enemy::ASG_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASG_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
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

