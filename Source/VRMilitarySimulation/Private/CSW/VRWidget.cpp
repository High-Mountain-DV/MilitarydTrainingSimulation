// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/VRWidget.h"

#include "Components/TextBlock.h"
#include "CSW/KeyboardWidgetActor.h"

void UVRWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UVRWidget::AppendSelectedInput(const FString& key)
{
	FString origin = SelectedInput->GetText().ToString();

	SelectedInput->SetText(FText::FromString(origin + key));
}

void UVRWidget::SpawnKeyboardWidgetActor()
{
	APlayerCameraManager* playerCam = GetOwningPlayerCameraManager();
	FRotator rot = (playerCam->GetActorForwardVector() * -1).Rotation();
	FVector loc = playerCam->K2_GetActorLocation() + playerCam->GetActorForwardVector() * 50;
	
	auto* keyboardActor = Cast<AKeyboardWidgetActor>(GetWorld()->SpawnActor(KeyboardWidgetFactory, &loc, &rot));
	if (keyboardActor)
	{
		keyboardActor->SetActorScale3D(FVector(0.1));
		keyboardActor->SetScreenWidget(this);
	}
}

void UVRWidget::SetVRWidgetActor(AVRWidgetActor* actor)
{
	VRWidgetActor = actor;
}
