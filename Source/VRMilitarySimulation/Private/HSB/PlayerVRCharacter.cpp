// Fill out your copyright notice in the Description page of Project Settings.


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
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CustomMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CustomMesh"));
	CustomMesh->SetupAttachment(GetMesh());
}

void APlayerVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerVRCharacter, RightCPPController);
	DOREPLIFETIME(APlayerVRCharacter, LeftCPPController);
}

// Called when the game starts or when spawned
void APlayerVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임 인스턴스에서 ID 가져오기
	UCSWGameInstance* GameInstance = Cast<UCSWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// 플레이어 ID를 액터 이름으로 설정
		// GetUserId()의 반환값을 FString으로 변환
		FString PlayerNickNameString(GameInstance->GetNickname());
		// SetActorLabel(PlayerNickNameString);
		Tags.Add(FName(PlayerNickNameString));

		PlayerNickName = PlayerNickNameString;

		int32 PlayerIDString(GameInstance->GetUserId());
		PlayerId = PlayerIDString;
	}

	
}

void APlayerVRCharacter::Server_UpdateControllerTransform_Implementation(
	const FTransform& RightTransform,
	const FTransform& LeftTransform)
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Im Server"));
	Multicast_UpdateControllerTransform(RightTransform, LeftTransform);
}

void APlayerVRCharacter::Multicast_UpdateControllerTransform_Implementation(
	const FTransform& RightTransform,
	const FTransform& LeftTransform)
{
	if (IsLocallyControlled()) return;

	if (RightCPPController)
	{
		RightCPPController->SetWorldTransform(RightTransform);
	}
	if (LeftCPPController)
	{
		LeftCPPController->SetWorldTransform(LeftTransform);
	}
}

void APlayerVRCharacter::CustomLoad()
{
	// 저장된 데이터를 로드하고 캐릭터에 반영
	UCSWGameInstance* LoadedGameInstance = Cast<UCSWGameInstance>(UGameplayStatics::LoadGameFromSlot(TEXT("CharacterSaveSlot"), 0));
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
			// 부대마크 머터리얼 로드 및 적용
			FString* MarkPath/* = LoadedGameInstance->변수명*/; 
			// 위장크림 머터리얼 로드 및 적용
			FString* CreamPath/* = LoadedGameInstance->변수명*/;
			// 악세서리 메쉬 로드 및 적용
			FString* AccPath/* = LoadedGameInstance->변수명*/;

			if (MarkPath && CreamPath && AccPath)
			{
				UMaterialInterface* LoadedMarkMat= LoadObject<UMaterialInterface>(nullptr, **MarkPath);
				UMaterialInterface* LoadedCreamMat = LoadObject<UMaterialInterface>(nullptr, **CreamPath);
				UStaticMesh* LoadedAccMesh = LoadObject<UStaticMesh>(nullptr, **AccPath);
				
				if (LoadedMarkMat && LoadedCreamMat && LoadedAccMesh)
				{
					ServerSetCustomData(LoadedMarkMat, LoadedCreamMat, LoadedAccMesh);
				}
			}
		}
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
	}
	if (CreamMat)
	{
		// 위장크림 적용
		GetMesh()->SetMaterial(1, CreamMat); // HeadGear_001.Mat
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
		}
	}
}

	// Called every frame
void APlayerVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector RightLoc;
		FVector LeftLoc;
		RightLoc = RightCPPController->GetComponentLocation();
		LeftLoc = LeftCPPController->GetComponentLocation();
		
//		UE_LOG(LogTemp, Warning, TEXT("X: %f , Y: %f, Z: %f"), RightLoc.X, RightLoc.Y, RightLoc.Z);
	}

	if (IsLocallyControlled())
	{
		// 타이머를 설정하여 주기적으로 컨트롤러 Transform을 업데이트
		
		UE_LOG(LogTemp, Warning, TEXT("Has Not Controlled"));
		if (RightCPPController && LeftCPPController)
		{
			Server_UpdateControllerTransform(
				RightCPPController->GetComponentTransform(),
				LeftCPPController->GetComponentTransform()
			);
			UE_LOG(LogTemp, Warning, TEXT("Has Controlled"));
		}
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

