// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CustomModel.h"

// Sets default values
ACustomModel::ACustomModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	GunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMeshComp"));
	GunMeshComp->SetupAttachment(MeshComp);

	AccMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AccMeshComp"));
	AccMeshComp->SetupAttachment(MeshComp, TEXT("mixamorig_HeadSocket"));
}

void ACustomModel::SetCustomizingPath(const FString& path, ECustomizingCategory category)
{
	if (category == MARK)
	{
		UMaterial* mat = Cast<UMaterial>(StaticLoadObject(
			UMaterial::StaticClass(),
			nullptr,
			*path
			));
		
		if (mat)
			MeshComp->SetMaterial(0, mat);
	}
	else if (category == CREAM)
	{
		UMaterial* mat = Cast<UMaterial>(StaticLoadObject(
			UMaterial::StaticClass(),
			nullptr,
			*path
			));
		
		if (mat)
			MeshComp->SetMaterial(1, mat);
	}
	else if (category == ACC)
	{
		UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(
			UStaticMesh::StaticClass(),
			nullptr,
			*path
			));

		if (mesh)
			AccMeshComp->SetStaticMesh(mesh);
	}
}

// Called when the game starts or when spawned
void ACustomModel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACustomModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

