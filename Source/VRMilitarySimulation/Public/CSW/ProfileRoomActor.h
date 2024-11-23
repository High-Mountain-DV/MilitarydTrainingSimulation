// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/RoomActor.h"
#include "ProfileRoomActor.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API AProfileRoomActor : public ARoomActor
{
	GENERATED_BODY()

public:
	
	virtual void OnRoomIn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnRoomOut(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<class AVRWidgetActor> InfoFactory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<class AVRWidgetActor> ReportFactory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<class AVRWidgetActor> CustomizingFactory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<class AVRWidgetActor> ReplayFactory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FTransform InfoSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FTransform ReportSpawnPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FTransform CustomizingSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FTransform ReplaySpawnPoint;

	UPROPERTY()
	TArray<AActor*> actors;
};
