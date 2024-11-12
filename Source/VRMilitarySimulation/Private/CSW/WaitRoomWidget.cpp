// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/WaitRoomWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSW/CSWGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void UWaitRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_GoLobby->OnClicked.AddDynamic(this, &UWaitRoomWidget::OnClick_GoLobby);
	Button_GameStart->OnClicked.AddDynamic(this, &UWaitRoomWidget::OnClick_GameStart);
	Button_GameReady->OnClicked.AddDynamic(this, &UWaitRoomWidget::OnClick_GameReady);

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc && pc->HasAuthority())
		Button_GameReady->SetVisibility(ESlateVisibility::Hidden);
	else
		Button_GameStart->SetVisibility(ESlateVisibility::Hidden);

	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		auto *SessionInfo = gi->GetSessionInfo();
		if (SessionInfo)
			MaxPlayerCnt = SessionInfo->SessionSettings.NumPublicConnections;

	}
}



void UWaitRoomWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 1. 다른 플레이어들의 정보를 알고싶다.
	TArray<TObjectPtr<APlayerState>> users = GetWorld()->GetGameState()->PlayerArray;

	// 2. 플레이어들의 이름을 다 모아서 
	FString names;
	for( APlayerState* user : users )
	{
		names.Append(FString::Printf(TEXT("%s\n"), *user->GetPlayerName()));
	}
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *names);
	// 3. 출력하고싶다.
	Txt_Users->SetText(FText::FromString(names));
	// if (users.Num() == MaxPlayerCnt && !GetWorld()->GetTimerManager().IsTimerActive(handle))
	// {
	// 	auto *pc = GetWorld()->GetFirstPlayerController();
	// 	if (pc->HasAuthority())
	// 	{
	// 		auto* gm = GetWorld()->GetAuthGameMode();
	// 		if (gm)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("ServerTravelCall!"));
	// 			gm->bUseSeamlessTravel = true;
	// 			// GetWorld()->ServerTravel(TEXT("/Game/MilitarySimulator/CSW/Maps/VRBattleMap?listen"));
	// 			GetWorld()->ServerTravel(TEXT("/Game/MilitarySimulator/JSG/Maps/MAIN_MAP?listen"));
	// 		}
	// 	}
	// }
}

void UWaitRoomWidget::OnClick_GoLobby()
{
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->ExitSession();
	}
}

void UWaitRoomWidget::OnClick_GameStart()
{
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	UE_LOG(LogTemp, Warning, TEXT("Click!"));

	if (gi)
	{
		gi->GoBattleField();
	}
}

void UWaitRoomWidget::OnClick_GameReady()
{
	
}


