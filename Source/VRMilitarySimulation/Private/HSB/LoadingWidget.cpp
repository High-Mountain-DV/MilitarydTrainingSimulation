// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/LoadingWidget.h"
#include "Kismet/GameplayStatics.h"

void ULoadingWidget::StartLevelLoading(FString LevelToLoad)
{
    // 현재 로딩 레벨 설정
    CurrentLoadingLevel = LevelToLoad;
    LoadingProgress = 0.0f;

    // 비동기 레벨 로딩
    UGameplayStatics::LoadStreamLevel(
        GetWorld(),
        FName(*CurrentLoadingLevel),
        true,   // bMakeVisibleAfterLoad
        false,  // bShouldBlockOnLoad
        FLatentActionInfo()
    );
}

float ULoadingWidget::GetLoadingProgress() const
{
    return LoadingProgress;
}