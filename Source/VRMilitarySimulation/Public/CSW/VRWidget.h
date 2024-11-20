// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UVRWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void AppendSelectedInput(const FString& key);
	void PopSelectedInput();
	void SpawnKeyboardWidgetActor();

	void SetVRWidgetActor(class AVRWidgetActor* actor);
	

protected:
	UPROPERTY()
	class UEditableText* SelectedInput;

	UPROPERTY()
	AVRWidgetActor* VRWidgetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<class AKeyboardWidgetActor> KeyboardWidgetFactory;
};
