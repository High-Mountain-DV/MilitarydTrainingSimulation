#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameModeBase.generated.h"

UENUM(BlueprintType)
enum class ETutorialStage : uint8
{
    Movement,
    Rotation,
    GunLoad,
    GunShoot,
    Completed
};

UCLASS()
class VRMILITARYSIMULATION_API ATutorialGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
    
public:
    ATutorialGameModeBase();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void StartTutorial();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void CheckTutorialProgress(ETutorialStage CurrentStage);

private:
    UPROPERTY()
    ETutorialStage CurrentTutorialStage;

    UFUNCTION()
    void ShowTutorialUI(ETutorialStage Stage);

    UFUNCTION()
    void UpdateTutorialProgress();
};
