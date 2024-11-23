// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/WaitRoomWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/WaitRoomWidget.h"

void AWaitRoomWidgetActor::AddPlayerPanel(const FString& nickname)
{
	UWaitRoomWidget* widget = Cast<UWaitRoomWidget>(WidgetComp->GetWidget());

	if (widget)
	{
		widget->AddPlayerPanel(nickname);
	}
}
