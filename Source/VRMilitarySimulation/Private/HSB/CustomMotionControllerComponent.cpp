// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/CustomMotionControllerComponent.h"
#include "Net/UnrealNetwork.h"

void UCustomMotionControllerComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwnerRole() == ROLE_Authority)
    {
        // 서버에서 PlayerIndex 할당
        SetPlayerIndex(GetOwner()->GetLocalRole());
    }
}

void UCustomMotionControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (IsActive() && PlayerIndex == GetOwner()->GetLocalRole())  // 본인 PlayerIndex_Custom일 때만 업데이트
    {
        FVector Position;
        FRotator Orientation;
        float WorldToMeters = GetWorld() ? GetWorld()->GetWorldSettings()->WorldToMeters : 100.0f;

        if (PollControllerState(Position, Orientation, WorldToMeters))
        {
            SetRelativeLocationAndRotation(Position, Orientation);
        }
    }
}

void UCustomMotionControllerComponent::SetPlayerIndex(int32 NewPlayerIndex)
{
    PlayerIndex = NewPlayerIndex;
    OnRep_PlayerIndex();
}

void UCustomMotionControllerComponent::OnRep_PlayerIndex()
{
    // 플레이어 인덱스 변경 시의 추가 로직을 여기에 추가
    UE_LOG(LogTemp, Log, TEXT("PlayerIndex가 %d로 변경되었습니다."), PlayerIndex);
}

void UCustomMotionControllerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCustomMotionControllerComponent, PlayerIndex);
}