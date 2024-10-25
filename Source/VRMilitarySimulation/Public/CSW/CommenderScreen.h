// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommenderScreen.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API ACommenderScreen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACommenderScreen();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddPlayerScreen(UMaterialInstanceDynamic* CamMtl) const;

	UFUNCTION(BlueprintCallable)
	void SelectScreen(int32 idx);

	UFUNCTION(BlueprintCallable)
	void SelectNextScreen();

	UFUNCTION(BlueprintCallable)
	void SelectPrevScreen();

	UFUNCTION(BlueprintCallable)
	void UnselectScreen();
	
private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USceneComponent* SceneComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	class UWidgetComponent* WidgetComp;
	
	
	
};
