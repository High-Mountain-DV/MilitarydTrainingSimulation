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
	FString MySessionName = TEXT("TEST");

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
	
	FString StringBase64Encode(const FString& str);
	FString StringBase64Decode(const FString& str);
};
