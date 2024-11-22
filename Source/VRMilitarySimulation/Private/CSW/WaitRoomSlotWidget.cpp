// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/WaitRoomSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWaitRoomSlotWidget::SetNickname(const FString& nickname, bool isCommender)
{
	Text_Nickname->SetText(FText::FromString(nickname));

	if (isCommender)
		ImageCommender->SetVisibility(ESlateVisibility::Visible);
	else
		ImageTrainee->SetVisibility(ESlateVisibility::Visible);
	
}
