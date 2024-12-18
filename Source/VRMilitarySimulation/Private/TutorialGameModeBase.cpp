#include "TutorialGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "HSB/PlayerVRCharacter.h"

ATutorialGameModeBase::ATutorialGameModeBase()
{
	CurrentTutorialStage = ETutorialStage::Movement;
}

void ATutorialGameModeBase::StartTutorial()
{
	ShowTutorialUI(CurrentTutorialStage);
}

void ATutorialGameModeBase::ShowTutorialUI(ETutorialStage Stage)
{
    // 각 단계별 UI 표시 로직
    switch (Stage)
    {
    case ETutorialStage::Movement:
        // 이동 튜토리얼 UI 표시
        break;
    case ETutorialStage::Rotation:
        // 회전 튜토리얼 UI 표시
        break;
    case ETutorialStage::GunLoad:
        // 총 장전 튜토리얼 UI 표시
        break;
    case ETutorialStage::GunShoot:
        // 총 발사 튜토리얼 UI 표시
        break;
    }
}

void ATutorialGameModeBase::CheckTutorialProgress(ETutorialStage CurrentStage)
{
    APlayerVRCharacter* PlayerCharacter = Cast<APlayerVRCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!PlayerCharacter) return;

    switch (CurrentStage)
    {
    case ETutorialStage::Movement:
        if (PlayerCharacter->HasCompletedMovementTutorial())
        {
            CurrentTutorialStage = ETutorialStage::Rotation;
            ShowTutorialUI(CurrentTutorialStage);
        }
        break;

    case ETutorialStage::Rotation:
        if (PlayerCharacter->HasCompletedRotationTutorial())
        {
            CurrentTutorialStage = ETutorialStage::GunLoad;
            ShowTutorialUI(CurrentTutorialStage);
        }
        break;

    case ETutorialStage::GunLoad:
        if (PlayerCharacter->HasCompletedGunLoadTutorial())
        {
            CurrentTutorialStage = ETutorialStage::GunShoot;
            ShowTutorialUI(CurrentTutorialStage);
        }
        break;

    case ETutorialStage::GunShoot:
        if (PlayerCharacter->HasCompletedGunShootTutorial())
        {
            CurrentTutorialStage = ETutorialStage::Completed;
            // 튜토리얼 완료 처리
        }
        break;
    }
}

void ATutorialGameModeBase::UpdateTutorialProgress()
{

}
