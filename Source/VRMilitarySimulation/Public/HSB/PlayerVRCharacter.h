#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerVRCharacter.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API APlayerVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerVRCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// 플레이어 태그 설정 함수
	UFUNCTION(Server, Reliable)
	void ServerSetNicknameAndID(const FString& nickname, int32 id);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetNicknameAndID(const FString& nickname, int32 id);
	
	
	// 커스텀 정보 로드할 함수
	UFUNCTION(BlueprintCallable)
	void CustomLoad();

	UFUNCTION(Server, Reliable)
	void ServerSetCustomData(UMaterialInterface* MarkMat, UMaterialInterface* CreamMat, UStaticMesh* AccMesh);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateCustomData(UMaterialInterface* MarkMat, UMaterialInterface* CreamMat, UStaticMesh* AccMesh);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 데미지 받는 함수
	UFUNCTION(BlueprintImplementableEvent)
	void DamageProcess(float Damage);
	
	UPROPERTY(BlueprintReadWrite)
	float Max_hp = 100;
	UPROPERTY(BlueprintReadWrite)
	float p_hp = Max_hp;

	// 쏘는 횟수 보내는 용
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int ShootingCnt; // 쏜 횟수

	UFUNCTION()
	int GetShootingCnt() const;

	// 플레이어 ID
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 PlayerId;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString PlayerNickName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsDead = false;
};
