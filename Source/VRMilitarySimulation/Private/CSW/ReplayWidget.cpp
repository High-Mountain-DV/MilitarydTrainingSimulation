// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReplayWidget.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Engine/DemoNetDriver.h"

void UReplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto demo = GetWorld()->GetDemoNetDriver())
	{
		float TotalTime = GetWorld()->GetDemoNetDriver()->GetDemoTotalTime();
		TimeSlider->SetMaxValue(TotalTime);
	}

	TimeSlider->OnMouseCaptureBegin.AddDynamic(this ,&UReplayWidget::OnClickTimeSliderBegin);
	TimeSlider->OnMouseCaptureEnd.AddDynamic(this, &UReplayWidget::OnClickTimeSliderEnd);
	
	ButtonRewind->OnPressed.AddDynamic(this, &UReplayWidget::OnClickRewind);
	ButtonPlay->OnPressed.AddDynamic(this, &UReplayWidget::OnClickPlay);
	ButtonSkip->OnPressed.AddDynamic(this, &UReplayWidget::OnClickSkip);
}

void UReplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// if (auto demo = GetWorld()->GetDemoNetDriver())
	// {
	// 	if (!demo->IsRecordingPaused())
	// 	{
	// 		TimeSlider->SetValue(demo->GetDemoCurrentTime());
	// 	}
	//
	// }
}

void UReplayWidget::OnClickTimeSliderBegin()
{
		UE_LOG(LogTemp, Warning, TEXT("%f"), TimeSlider->GetValue());
	if (auto demo = GetWorld()->GetDemoNetDriver())
	{
		demo->SetDemoCurrentTime(TimeSlider->GetValue());
	}
}

void UReplayWidget::OnClickTimeSliderEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("%f"), TimeSlider->GetValue());

	if (auto demo = GetWorld()->GetDemoNetDriver())
	{
		demo->SetDemoCurrentTime(TimeSlider->GetValue());
	}
}

void UReplayWidget::OnClickPlay()
{
	if (auto demo = GetWorld()->GetDemoNetDriver())
	{
		auto settings = GetWorld()->GetWorldSettings(); 
		if (settings)
		{
			if (settings->GetPauserPlayerState()) // isPaused
				settings->SetPauserPlayerState(nullptr);
			else
				settings->SetPauserPlayerState(GetWorld()->GetFirstPlayerController()->PlayerState);
		}
	}
}

void UReplayWidget::OnClickRewind()
{
	if (auto demo = GetWorld()->GetDemoNetDriver())
	{
		// demo->GotoTimeInSeconds(std::clamp(demo->GetDemoCurrentTime() - 5.f, 0.f, demo->GetDemoTotalTime()));
		demo->SetDemoCurrentTime(std::clamp(demo->GetDemoCurrentTime() - 5.f, 0.f, demo->GetDemoTotalTime()));

	}
}

void UReplayWidget::OnClickSkip()
{
	if (auto demo = GetWorld()->GetDemoNetDriver())
	{
		// demo->GotoTimeInSeconds(std::clamp(demo->GetDemoCurrentTime() + 5.f, 0.f, demo->GetDemoTotalTime()));
		demo->SetDemoCurrentTime(std::clamp(demo->GetDemoCurrentTime() + 5.f, 0.f, demo->GetDemoTotalTime()));

	}
}

