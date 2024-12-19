// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/LoadingGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ALoadingGameMode::ALoadingGameMode()
{
}

void ALoadingGameMode::CreateLoadingWidget()
{
    // 로딩 위젯 생성
    if (LoadingWidgetClass)
    {
        LoadingWidget = CreateWidget<UUserWidget>(
            GetWorld(),
            LoadingWidgetClass
        );

        if (LoadingWidget)
        {
            // 뷰포트에 로딩 위젯 추가
            LoadingWidget->AddToViewport();
        }
    }
}


void ALoadingGameMode::CompleteLoading_EnterTutorial()
{
    // 로딩 위젯 제거
    if (LoadingWidget)
    {
        LoadingWidget->RemoveFromParent();
    }

    // 튜토리얼 레벨로 전환
    UGameplayStatics::OpenLevel(
        GetWorld(),
        FName("Tutorial_Map")
    );
}
