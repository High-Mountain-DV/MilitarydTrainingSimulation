// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "MilitaryVRPawn.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API AMilitaryVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMilitaryVRPawn();
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//// Transform 업데이트를 위한 타이머 핸들
	//FTimerHandle UpdateTimerHandle;

	//// 컨트롤러 Transform 업데이트 간격 (초)
 //   UPROPERTY(EditAnywhere, Category = "VR|Network")
 //   float ControllerUpdateInterval;

	//// 서버에 Transform 업데이트 요청
 //   UFUNCTION(Server, Reliable)
 //   void Server_UpdateControllerTransform(const FTransform& RightTransform, const FTransform& LeftTransform);

 //   // 모든 클라이언트에 Transform 브로드캐스트
 //   UFUNCTION(NetMulticast, Reliable)
 //   void Multicast_UpdateControllerTransform(const FTransform& RightTransform, const FTransform& LeftTransform);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//// Motion Controllers
 //   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "VR")
 //   UMotionControllerComponent* RightController;

 //   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "VR")
 //   UMotionControllerComponent* LeftController;

	// 데미지 받는 함수
	UFUNCTION(BlueprintCallable)
	void DamageProcess(float Damage);
	
	UPROPERTY(BlueprintReadWrite)
	float Max_hp = 40;
	UPROPERTY(BlueprintReadWrite)
	float p_hp = Max_hp;

private:
	
public:
	//// Blueprint 이벤트를 위한 델리게이트
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectGrabbed, UPrimitiveComponent*, GrabbedComponent);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectReleased, UPrimitiveComponent*, ReleasedComponent);
	
};
