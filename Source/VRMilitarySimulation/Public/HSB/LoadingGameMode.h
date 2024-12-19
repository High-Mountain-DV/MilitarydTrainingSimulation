// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoadingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ALoadingGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    ALoadingGameMode();

    // 로딩 화면 위젯 생성
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CreateLoadingWidget();

    // 로딩 완료 후 레벨 전환
    UFUNCTION(BlueprintCallable, Category = "Level")
    void CompleteLoading_EnterTutorial();

protected:
    // 로딩 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UUserWidget> LoadingWidgetClass;

private:
    // 로딩 위젯 인스턴스
    UPROPERTY()
    class UUserWidget* LoadingWidget;
};
