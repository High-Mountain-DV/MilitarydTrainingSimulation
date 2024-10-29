// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_EnemyMagazine.h"

// Sets default values
ASG_EnemyMagazine::ASG_EnemyMagazine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASG_EnemyMagazine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASG_EnemyMagazine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

