// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpActor.h"
#include "GameFramework/Actor.h"
#include "VRWidgetActor.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class VRMILITARYSIMULATION_API AVRWidgetActor : public AHttpActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AppendSelectedText(const FString& key);

	UFUNCTION()
	void ClickWidgetRight();

	UFUNCTION()
	void ClickWidgetLeft();

	UFUNCTION()
	void UnclickWidgetRight();

	UFUNCTION()
	void UnclickWidgetLeft();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= Component, meta=(AllowPrivateAccess))
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= Component, meta=(AllowPrivateAccess))
	class UWidgetComponent* WidgetComp;

private:
	UPROPERTY()
	class UWidgetInteractionComponent* WidgetInteractRefLeft;

	UPROPERTY()
	class UWidgetInteractionComponent* WidgetInteractRefRight;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input , meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WidgetInterationMappingContext;

	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input , meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Widget_Interact_Left;

	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input , meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Widget_Interact_Right;
	
	
};
