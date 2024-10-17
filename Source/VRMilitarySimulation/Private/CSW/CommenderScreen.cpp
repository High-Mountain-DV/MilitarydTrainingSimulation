// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CommenderScreen.h"

#include "Components/WidgetComponent.h"
#include "CSW/CommenderScreenWidget.h"

// Sets default values
ACommenderScreen::ACommenderScreen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);
	
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACommenderScreen::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ACommenderScreen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACommenderScreen::AddPlayerScreen(UMaterialInstanceDynamic* CamMtl) const
{
	auto* widget = Cast<UCommenderScreenWidget>(WidgetComp->GetWidget());
	if (widget)
		widget->AddPlayerScreen(CamMtl);
}

