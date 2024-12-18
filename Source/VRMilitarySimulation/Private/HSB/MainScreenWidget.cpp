// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/MainScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "HSB/LoadingWidget.h"
#include "HSB/LoadingGameMode.h"

void UMainScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Enter->OnClicked.AddDynamic(this, &UMainScreenWidget::Enter_Main);
	Btn_Tutorial->OnClicked.AddDynamic(this, &UMainScreenWidget::Enter_Tutorial);
}

void UMainScreenWidget::Enter_Main()
{
    // 문 열리는 로직
}

void UMainScreenWidget::Enter_Tutorial()
{
    // 게임 인스턴스 가져오기
    ULoadingWidget* GameInstance =
        Cast<ULoadingWidget>(UGameplayStatics::GetGameInstance(GetWorld()));

    // 로딩 게임 모드 가져오기
    ALoadingGameMode* LoadingGameMode =
        Cast<ALoadingGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    if (GameInstance && LoadingGameMode)
    {
        // 로딩 위젯 생성
        LoadingGameMode->CreateLoadingWidget();

        // 레벨 로딩 시작
        GameInstance->StartLevelLoading("MainGameLevel");

        // 일정 시간 후 로딩 완료 (실제로는 비동기 로딩 완료 이벤트에 연결)
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            LoadingGameMode,
            &ALoadingGameMode::CompleteLoading_EnterTutorial,
            3.0f,  // 로딩 시간 (테스트용)
            false
        );
    }
}
