// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/SessionsOSSAdapter.h"
#include "CSWGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	int32 maxPlayerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 currentPlayerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 pingMS;

	int32 index;

	FString ToString()
	{
		return FString::Printf(TEXT("%d)[%s][%s] (%d / %d) -> %dms"), index, *roomName, *hostName, currentPlayerCount, maxPlayerCount, pingMS);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature , const struct FRoomInfo& , info);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindSignature , bool, value);

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UCSWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	

	IOnlineSessionPtr SessionInterface;
	FString MySessionName;
	FString CurrentSessionName;
	FString GenerateTimestampedSessionName() const ;
	FSearchSignature OnSearchSignatureCompleteDelegate;

	// 찾기를 위한 델리게이트...
	// FFindSignature OnFindSignatureCompleteDelegate;
	
	void CreateMySession(const FString& roomName, int32 playerCount);
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	// 방찾기 요청
	void FindOtherSessions();
	// 방찾기 응답
	void OnMyFindSessionsCompleteDelegates(bool bWasSuccessful);
	
	// 방입장 요청
	void JoinMySession(int32 index);
	// 방입장 응답
	void OnMyJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type EOnJoinSessionCompleteResult);


	// 방퇴장 요청 -> UI에서 호출
	void ExitSession();

	UFUNCTION(Server, Reliable)
	void ServerRPCExitSession();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExitSession();
	
	// 방퇴장 응답
	void OnMyDestroySessionComplete(FName SessionName , bool bWasSuccessful);

	void OnMySessionParticipantsChange(FName SessionName, const FUniqueNetId& UniqueId, bool bJoined);

	//참여중인 세션 정보 가져오기
	FNamedOnlineSession* GetSessionInfo();
	
	FString StringBase64Encode(const FString& str);
	FString StringBase64Decode(const FString& str);




	// replay system ========================
	FString RecordingName = "ExampleRecording";
	FString FriendlyRecordingName = "Example Recording";
	
	UFUNCTION(BlueprintCallable)
	void StartRecording();

	UFUNCTION(BlueprintCallable)
	void StopRecording();

	UFUNCTION(BlueprintCallable)
	void ReplayRecording();

	// travel ============================
	void GoLobby();
	
	void GoWaitingRoom();

	void GoBattleField();
	
	void GoReportRoom();

	// setter, getter ====================
	void SetUserToken(const FString& token);
	void SetUserId(int32 id);
	void SetUserNickname(const FString& nickname);
	
	const int32& GetUserId() const;

	UFUNCTION(BlueprintCallable)
	const FString& GetNickname() const;

	const FString& GetUserToken() const;
private:
	FString UserToken = "Bearer eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiIzIiwiaWF0IjoxNzMxMzI1MDI5LCJleHAiOjE3MzE0MTE0Mjl9.EA9bvR7taldniSf_nwrvfLoEEdwJggZ0_E9HbTPUI-k";
	int32 UserId = 3;
	FString UserNickname = "1";
	FString RecentCombatData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FString LobbyURL = "/Game/MilitarySimulator/CSW/Maps/VRLobbyMap";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FString WaitingRoomURL = "/Game/MilitarySimulator/CSW/Maps/VRWaitingMap";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FString BattleFieldURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FString ReportRoomURL = "/Game/MilitarySimulator/CSW/Maps/VRReportRoomMap";
};
