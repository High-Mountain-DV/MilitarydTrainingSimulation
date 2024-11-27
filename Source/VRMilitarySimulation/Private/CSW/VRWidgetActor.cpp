// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/VRWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "CSW/VRWidget.h"

// Sets default values
AVRWidgetActor::AVRWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("MainWidgetComp"));
	WidgetComp->SetupAttachment(SceneComp);

	//ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("/Game/MilitarySimulator/SHN/UI/M_testWdiget_Inst.M_testWdiget_Inst"));
	//WidgetComp->SetMaterial(0, mat.Object);
}

// Called when the game starts or when spawned
void AVRWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	// auto *pc = GetWorld()->GetFirstPlayerController();
	// if (pc != nullptr)
	// {
	// 	EnableInput(pc);
	// 	if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()) )
	// 	{
	// 		Subsystem->AddMappingContext(WidgetInterationMappingContext , 0);
	// 	}
	//
	// 	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	// 	{
	// 		EnhancedInputComponent->BindAction(IA_Widget_Interact_Left, ETriggerEvent::Started, this, &AVRWidgetActor::ClickWidgetLeft);
	// 		EnhancedInputComponent->BindAction(IA_Widget_Interact_Right, ETriggerEvent::Started, this, &AVRWidgetActor::ClickWidgetRight);
	//
	// 		EnhancedInputComponent->BindAction(IA_Widget_Interact_Left, ETriggerEvent::Completed, this, &AVRWidgetActor::UnclickWidgetLeft);
	// 		EnhancedInputComponent->BindAction(IA_Widget_Interact_Right, ETriggerEvent::Completed, this, &AVRWidgetActor::UnclickWidgetRight);
	// 	}
	//
	// 	if (pc->GetPawn())
	// 	{
	// 		TArray<UActorComponent*> comps = pc->GetPawn()->K2_GetComponentsByClass(UWidgetInteractionComponent::StaticClass());
	// 		if (comps.Num() == 2)
	// 		{
	// 			WidgetInteractRefLeft = Cast<UWidgetInteractionComponent>(comps[0]);
	// 			WidgetInteractRefRight = Cast<UWidgetInteractionComponent>(comps[1]);
	// 		}
	// 	}
	// }
}

// Called every frame
void AVRWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRWidgetActor::AppendSelectedText(const FString& key)
{
	UVRWidget* widget = Cast<UVRWidget>(WidgetComp->GetWidget());

	if (widget)
		widget->AppendSelectedInput(key);
}

void AVRWidgetActor::ClickWidgetRight()
{
	WidgetInteractRefRight->PressPointerKey(EKeys::LeftMouseButton);
}

void AVRWidgetActor::ClickWidgetLeft()
{
	WidgetInteractRefLeft->PressPointerKey(EKeys::LeftMouseButton);
}

void AVRWidgetActor::UnclickWidgetRight()
{
	WidgetInteractRefRight->ReleasePointerKey(EKeys::LeftMouseButton);
}

void AVRWidgetActor::UnclickWidgetLeft()
{
	WidgetInteractRefLeft->ReleasePointerKey(EKeys::LeftMouseButton);
}

