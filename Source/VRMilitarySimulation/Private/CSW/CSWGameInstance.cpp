// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CSWGameInstance.h"

#include <string>

#include "GameFramework/GameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

// #include "Interfaces/OnlineSessionInterface.h"

void UCSWGameInstance::Init()
{
	Super::Init();

	MySessionName = GenerateTimestampedSessionName();
	if (auto *subSystem = IOnlineSubsystem::Get())
		SessionInterface = subSystem->GetSessionInterface();

	// 방생성 응답
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCSWGameInstance::OnMyCreateSessionComplete);

	// 방찾기 응답
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this , &UCSWGameInstance::OnMyFindSessionsCompleteDelegates);
	
	// 방입장 응답
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this , &UCSWGameInstance::OnMyJoinSessionComplete);
	
	// 방퇴장 응답
	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this , &UCSWGameInstance::OnMyDestroySessionComplete);

	// 세션 참여자 변동 알림
	SessionInterface->OnSessionParticipantsChangeDelegates.AddUObject(this, &UCSWGameInstance::OnMySessionParticipantsChange);
}

FString UCSWGameInstance::GenerateTimestampedSessionName() const
{
	// 현재 시간 얻기
	FDateTime Now = FDateTime::Now();
    
	// 타임스탬프 형식으로 변환 (연도, 월, 일, 시, 분, 초)
	FString Timestamp = Now.ToString(TEXT("%Y%m%d%H%M%S"));

	// 세션 이름 생성
	FString SessionName = FString::Printf(TEXT("Session_%s"), *Timestamp);

	return SessionName;
}

void UCSWGameInstance::CreateMySession(const FString& roomName, int32 playerCount)
{
	if (SessionInterface->GetNamedSession(FName(MySessionName)) != nullptr)
	{
		SessionInterface->DestroySession(FName(MySessionName));
	}
	FOnlineSessionSettings settings;

	// 1. 전용서버를 사용하는가?
	settings.bIsDedicated = false;

	// 2. 랜선(Lan)으로 매치하는가?
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	settings.bIsLANMatch = subsysName == "NULL";

	// 3. 매칭이 공개(true)혹은 비공개(false, 초대를 통해서 매칭)
	settings.bShouldAdvertise = true;

	// 4. 유저의 상태 정보(온라인/자리비움/등등) 사용 여부
	settings.bUsesPresence = true;
	
	// 5. 중간에 난입 가능한가?
	settings.bAllowJoinViaPresence = true;
	settings.bAllowJoinInProgress = true;

	// 6. 최대 인원수
	settings.NumPublicConnections = playerCount;

	// 7. 커스텀 정보

	settings.Set(FName("ROOM_NAME") , StringBase64Encode(roomName) , EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME") , StringBase64Encode(MySessionName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	SessionInterface->CreateSession(*netID , FName(MySessionName), settings);

	// PRINTLOG(TEXT("Create Session Start roomNamd : %s / hostName : %s") , *roomName , *MySessionName);
}

void UCSWGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMyCreateSessionComplete is Success!!"));
		UE_LOG(LogTemp, Warning, TEXT("%s\n"), *(IOnlineSubsystem::Get()->GetSubsystemName()).ToString())
		// 서버가 여행을 떠나고싶다.
		CurrentSessionName = MySessionName;
		GoWaitingRoom();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMyCreateSessionComplete is Failed!!"));
	}
}

void UCSWGameInstance::FindOtherSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch);

	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSearch->MaxSearchResults = 40;

	SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());
}

void UCSWGameInstance::OnMyFindSessionsCompleteDelegates(bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;

		for (int32 i=0 ; i< results.Num() ; i++)
		{
			FOnlineSessionSearchResult ret = results[i];
			if (false == ret.IsValid())
			{
				continue;
			}

			FRoomInfo roomInfo;
			roomInfo.index = i;

			// 방이름
			FString roomName;
			ret.Session.SessionSettings.Get(FName("ROOM_NAME") , roomName);
			roomInfo.roomName = StringBase64Decode(roomName);
			// 호스트이름
			FString hostName;
			ret.Session.SessionSettings.Get(FName("HOST_NAME") , hostName);
			roomInfo.hostName = StringBase64Decode(hostName);
			// 최대 플레이어 수
			roomInfo.maxPlayerCount = ret.Session.SessionSettings.NumPublicConnections;
			// 입장한 플레이어 수(최대 - 남은)
			roomInfo.currentPlayerCount = roomInfo.maxPlayerCount - ret.Session.NumOpenPublicConnections;
			// 핑 정보
			roomInfo.pingMS = ret.PingInMs;

			if ( OnSearchSignatureCompleteDelegate.IsBound() )
				OnSearchSignatureCompleteDelegate.Broadcast(roomInfo);

			UE_LOG(LogTemp, Warning, TEXT("%s") , *roomInfo.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMyFindSessionsCompleteDelegates is Failed!!!"));
	}
}

void UCSWGameInstance::JoinMySession(int32 index)
{
	auto result = SessionSearch->SearchResults[index];
	SessionInterface->JoinSession(0 , FName(MySessionName) , result);
}

void UCSWGameInstance::OnMyJoinSessionComplete(FName SessionName,
	EOnJoinSessionCompleteResult::Type EOnJoinSessionCompleteResult)
{
	if ( EOnJoinSessionCompleteResult::Success == EOnJoinSessionCompleteResult )
	{
		// 서버가 있는 레벨로 여행을 떠나고 싶다.
		auto* pc = GetWorld()->GetFirstPlayerController();

		FString url;
		CurrentSessionName = SessionName.ToString();
		SessionInterface->GetResolvedConnectString(SessionName, url);
		if (false == url.IsEmpty())
		{
			pc->ClientTravel(url , ETravelType::TRAVEL_Absolute);
		}
	}
}

void UCSWGameInstance::ExitSession()
{
	ServerRPCExitSession();
}

void UCSWGameInstance::ServerRPCExitSession_Implementation()
{
	MulticastRPCExitSession();
}

void UCSWGameInstance::MulticastRPCExitSession_Implementation()
{
	// 방퇴장 요청
	SessionInterface->DestroySession(FName(MySessionName));
}

void UCSWGameInstance::OnMyDestroySessionComplete(FName SessionName , bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		auto* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(ReportRoomURL, ETravelType::TRAVEL_Absolute);
	}
}


void UCSWGameInstance::OnMySessionParticipantsChange(FName SessionName, const FUniqueNetId& UniqueId, bool bJoined)
{
	if (bJoined)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player joined: %s"), *UniqueId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player left: %s"), *UniqueId.ToString());
	}
}

FNamedOnlineSession* UCSWGameInstance::GetSessionInfo()
{
	// 온라인 서브시스템에 접근
	if (SessionInterface.IsValid())
	{
		// 세션 이름으로 세션 정보 가져오기 (일반적으로 세션 이름은 NAME_GameSession)
		return SessionInterface->GetNamedSession(FName(CurrentSessionName));
	}
	return nullptr;
}

FString UCSWGameInstance::StringBase64Encode(const FString& str)
{
	// Set 할 때 : FString -> UTF8 (std::string) -> TArray<uint8> -> base64 로 Encode
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData = TArray<uint8>((uint8*)(utf8String.c_str()) ,
	utf8String.length());
	return FBase64::Encode(arrayData);
}

FString UCSWGameInstance::StringBase64Decode(const FString& str)
{
	// Get 할 때 : base64 로 Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode(str , arrayData);
	std::string ut8String((char*)(arrayData.GetData()) , arrayData.Num());
	return UTF8_TO_TCHAR(ut8String.c_str());
}

void UCSWGameInstance::SetUserToken(const FString& token)
{
	UserToken = token;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *token);
}

void UCSWGameInstance::SetUserId(int32 id)
{
	UserId = id;
}

void UCSWGameInstance::SetUserNickname(const FString& nickname)
{
	UserNickname = nickname;
}

void UCSWGameInstance::StartRecording()
{
	StartRecordingReplay(RecordingName, FriendlyRecordingName);	
}

void UCSWGameInstance::StopRecording()
{
	StopRecordingReplay();
}

void UCSWGameInstance::ReplayRecording()
{
	PlayReplay(RecordingName, nullptr);
}

void UCSWGameInstance::GoLobby()
{
	ResetTraineesId();
	GetWorld()->ServerTravel(LobbyURL);
}

void UCSWGameInstance::GoWaitingRoom()
{
	GetWorld()->ServerTravel(WaitingRoomURL + "?listen");
}

void UCSWGameInstance::GoBattleField()
{
	auto* gm = GetWorld()->GetAuthGameMode();

	if (gm)
	{
		gm->bUseSeamlessTravel = true;
		FString path = TEXT("/Game/MilitarySimulator/SHN/MAIN_MAP");
		GetWorld()->ServerTravel(path + "?listen");
	}
}

void UCSWGameInstance::GoReportRoom()
{
	GetWorld()->ServerTravel(ReportRoomURL);
}

const int32& UCSWGameInstance::GetUserId() const
{
	return UserId;
}


const FString& UCSWGameInstance::GetNickname() const
{
	return UserNickname;
}

const FString& UCSWGameInstance::GetUserToken() const
{
	return UserToken;
}

void UCSWGameInstance::AppendTraineesId(int32 id)
{
	TraineesId.Add(id);
}

void UCSWGameInstance::ResetTraineesId()
{
	TraineesId.Empty();
}

bool UCSWGameInstance::IsCommender() const
{
	return !TraineesId.IsEmpty();	
}

const TArray<int32>& UCSWGameInstance::GetTraineesId() const
{
	return TraineesId;
}


