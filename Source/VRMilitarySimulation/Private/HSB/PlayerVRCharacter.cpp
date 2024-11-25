#include "HSB/PlayerVRCharacter.h"
#include "MotionControllerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/CoreNet.h"
#include "CSW/CSWGameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystemTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

// Sets default values
APlayerVRCharacter::APlayerVRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void APlayerVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임 인스턴스에서 ID 가져오기
	UCSWGameInstance* GameInstance = Cast<UCSWGameInstance>(GetGameInstance());
	if (GameInstance && IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("nickname: %s, id: %d"), *(GameInstance->GetNickname()), GameInstance->GetUserId());
		ServerSetNicknameAndID(GameInstance->GetNickname(), GameInstance->GetUserId());
	}
}

void APlayerVRCharacter::ServerSetNicknameAndID_Implementation(const FString& nickname, int32 id)
{
	MulticastSetNicknameAndID(nickname, id);
}

void APlayerVRCharacter::MulticastSetNicknameAndID_Implementation(const FString& nickname, int32 id)
{
	// 플레이어 ID를 액터 이름으로 설정
	// GetUserId()의 반환값을 FString으로 변환

	UE_LOG(LogTemp, Warning, TEXT("multicast nickname: %s, id: %d"), *nickname, id);
	

	Tags.Add(FName(nickname));
	PlayerNickName = nickname;
	PlayerId = id;
}

void APlayerVRCharacter::CustomLoad()
{
	// 저장된 데이터를 로드하고 캐릭터에 반영
	UCSWGameInstance* LoadedGameInstance = Cast<UCSWGameInstance>(UGameplayStatics::GetGameInstance(this));

	const FCustomizingPath& CustomizingPaths = LoadedGameInstance->GetCustomizingPath();

	if (LoadedGameInstance)
	{
		FString SaveGamePath = FPaths::ProjectSavedDir();  // Save 디렉토리 경로 확인
	 	UE_LOG(LogTemp, Log, TEXT("Save game loaded from: %s"), *SaveGamePath);
	
	 	// 레벨 들어가면 커스텀 정보들 적용됨
	 	UWorld* World = GetWorld();
	 	if (!World)
	 		return;
	 	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
	 	if (CurrentLevelName == "MainMap")
	 	{
			// Retrieve paths for Mark, Cream, and Accessory
			FString MarkPath = CustomizingPaths.MarkPath;
			FString CreamPath = CustomizingPaths.CreamPath;
			FString AccPath = CustomizingPaths.AccPath;
	
			UMaterialInterface* LoadedMarkMat = nullptr;
			UMaterialInterface* LoadedCreamMat = nullptr;
			UStaticMesh* LoadedAccMesh = nullptr;

			if (!MarkPath.IsEmpty())
			{
				LoadedMarkMat = LoadObject<UMaterialInterface>(nullptr, *MarkPath);
				if (!LoadedMarkMat)
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to load Mark Material from path: %s"), *MarkPath);
				}
			}

			if (!CreamPath.IsEmpty())
			{
				LoadedCreamMat = LoadObject<UMaterialInterface>(nullptr, *CreamPath);
				if (!LoadedCreamMat)
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to load Cream Material from path: %s"), *CreamPath);
				}
			}

			if (!AccPath.IsEmpty())
			{
				LoadedAccMesh = LoadObject<UStaticMesh>(nullptr, *AccPath);
				if (!LoadedAccMesh)
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to load Accessory Mesh from path: %s"), *AccPath);
				}
			}

			ServerSetCustomData(LoadedMarkMat, LoadedCreamMat, LoadedAccMesh);
	 	}
		else return; // "MainMap"이 아니면 return
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
	}
}

void APlayerVRCharacter::ServerSetCustomData_Implementation(UMaterialInterface * MarkMat, UMaterialInterface * CreamMat, UStaticMesh * AccMesh)
{
	MulticastUpdateCustomData(MarkMat, CreamMat, AccMesh); // 모든 클라이언트에게 전송
}

void APlayerVRCharacter::MulticastUpdateCustomData_Implementation(UMaterialInterface * MarkMat, UMaterialInterface * CreamMat, UStaticMesh * AccMesh)
{
	// ServerSetCustomData 내용과 동일
	if (MarkMat)
	{
		// 부대마크 적용
		GetMesh()->SetMaterial(0, MarkMat); // Body_001.Mat

		UE_LOG(LogTemp, Log, TEXT("Mark material applied."));
	}
	if (CreamMat)
	{
		// 위장크림 적용
		GetMesh()->SetMaterial(1, CreamMat); // HeadGear_001.Mat

		UE_LOG(LogTemp, Log, TEXT("Cream material applied."));
	}
	if (AccMesh)
	{
		// 악세사리 소켓에 부착 및 적용
		USkeletalMeshComponent* SkeletalMesh = GetMesh(); // 캐릭터 메쉬
		if (SkeletalMesh)
		{
			// 소켓 이름 설정
			FName SocketName = "AccSocket";
			UStaticMeshComponent* AccMeshComponent = NewObject<UStaticMeshComponent>(this);
			AccMeshComponent->SetStaticMesh(AccMesh);
			AccMeshComponent->SetupAttachment(SkeletalMesh, SocketName);
			AccMeshComponent->RegisterComponent();

			UE_LOG(LogTemp, Log, TEXT("Accessory mesh attached to socket: %s"), *SocketName.ToString());
		}
	}
}

	// Called every frame
void APlayerVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("my userID: %d, my nickname: %s"), PlayerId, *PlayerNickName);
	}
}

// Called to bind functionality to input
void APlayerVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

int APlayerVRCharacter::GetShootingCnt() const
{
	return ShootingCnt;
}

