// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 레벨 로딩 시작 메서드
    UFUNCTION(BlueprintCallable, Category = "Level Loading")
    void StartLevelLoading(FString LevelToLoad);

    // 현재 로딩 상태 가져오기
    UFUNCTION(BlueprintPure, Category = "Level Loading")
    float GetLoadingProgress() const;

private:
    // 현재 로딩 중인 레벨
    UPROPERTY()
    FString CurrentLoadingLevel;

    // 로딩 진행 상태
    UPROPERTY()
    float LoadingProgress;
};
