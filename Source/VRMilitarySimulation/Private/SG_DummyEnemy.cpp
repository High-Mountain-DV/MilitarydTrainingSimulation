// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_DummyEnemy.h"

// Sets default values
ASG_DummyEnemy::ASG_DummyEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetRelativeTransform(FTransform(FRotator(0, -90, 0), FVector(0, 0, -90), FVector(1)));
}

// Called when the game starts or when spawned
void ASG_DummyEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASG_DummyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

