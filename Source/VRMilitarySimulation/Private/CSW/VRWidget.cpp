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

void UVRWidget::PopSelectedInput()
{
	FString origin = SelectedInput->GetText().ToString();

	if (origin.Len() > 0)
	{
		origin.RemoveAt(origin.Len() - 1);
		SelectedInput->SetText(FText::FromString(origin));
	}
}

void UVRWidget::SpawnKeyboardWidgetActor()
{
	APlayerCameraManager* playerCam = GetOwningPlayerCameraManager();
	FRotator rot = (playerCam->GetActorForwardVector() * -1).Rotation();
	rot.Pitch += 20;
	FVector loc = playerCam->K2_GetActorLocation() + playerCam->GetActorForwardVector() * 70 + FVector(0, 0, -35);
	
	auto* keyboardActor = Cast<AKeyboardWidgetActor>(GetWorld()->SpawnActor(KeyboardWidgetFactory, &loc, &rot));
	if (keyboardActor)
	{
		keyboardActor->SetActorScale3D(FVector(0.05));
		keyboardActor->SetScreenWidget(this);
	}
}

void UVRWidget::SetVRWidgetActor(AVRWidgetActor* actor)
{
	VRWidgetActor = actor;
}
